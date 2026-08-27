#include "hal/esp32_twai_can.h"

#include <Arduino.h>
#include "driver/twai.h"

namespace vag_data {
namespace {

constexpr std::uint32_t kRequiredAlerts = TWAI_ALERT_RX_QUEUE_FULL |
                                          TWAI_ALERT_RX_FIFO_OVERRUN |
                                          TWAI_ALERT_BUS_OFF |
                                          TWAI_ALERT_TX_FAILED;

bool timingForBitrate(std::uint32_t bitrate, twai_timing_config_t& timing) {
  switch (bitrate) {
    case 10000:
      timing = TWAI_TIMING_CONFIG_10KBITS();
      return true;
    case 20000:
      timing = TWAI_TIMING_CONFIG_20KBITS();
      return true;
    case 25000:
      timing = TWAI_TIMING_CONFIG_25KBITS();
      return true;
    case 50000:
      timing = TWAI_TIMING_CONFIG_50KBITS();
      return true;
    case 100000:
      timing = TWAI_TIMING_CONFIG_100KBITS();
      return true;
    case 125000:
      timing = TWAI_TIMING_CONFIG_125KBITS();
      return true;
    case 250000:
      timing = TWAI_TIMING_CONFIG_250KBITS();
      return true;
    case 500000:
      timing = TWAI_TIMING_CONFIG_500KBITS();
      return true;
    case 800000:
      timing = TWAI_TIMING_CONFIG_800KBITS();
      return true;
    case 1000000:
      timing = TWAI_TIMING_CONFIG_1MBITS();
      return true;
    default:
      return false;
  }
}

bool validConfig(const HardwareConfig& config) {
  return config.canTxGpio >= 0 && config.canRxGpio >= 0 && config.canBitrate != 0;
}

}  // namespace

Esp32TwaiCan::~Esp32TwaiCan() {
  stop();
}

CanStatus Esp32TwaiCan::initialize(const HardwareConfig& config) {
  if (!validConfig(config)) {
    return CanStatus::InvalidConfig;
  }

  twai_timing_config_t timing{};
  if (!timingForBitrate(config.canBitrate, timing)) {
    return CanStatus::InvalidConfig;
  }

  if (installed_ || started_) {
    const CanStatus cleanup = stop();
    if (cleanup != CanStatus::Ok) {
      return cleanup;
    }
  }

  twai_general_config_t general = TWAI_GENERAL_CONFIG_DEFAULT(
      static_cast<gpio_num_t>(config.canTxGpio),
      static_cast<gpio_num_t>(config.canRxGpio), TWAI_MODE_NORMAL);
  general.rx_queue_len = 16;
  general.alerts_enabled = kRequiredAlerts;
  twai_filter_config_t filter = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  if (twai_driver_install(&general, &timing, &filter) != ESP_OK) {
    return CanStatus::InvalidConfig;
  }
  installed_ = true;

  if (twai_start() != ESP_OK) {
    if (twai_driver_uninstall() != ESP_OK) {
      installed_ = true;
      started_ = false;
      return CanStatus::DriverError;
    }
    installed_ = false;
    return CanStatus::InvalidConfig;
  }
  started_ = true;
  return CanStatus::Ok;
}

CanStatus Esp32TwaiCan::stop() {
  if (!installed_) {
    return CanStatus::Ok;
  }

  twai_status_info_t status{};
  if (twai_get_status_info(&status) != ESP_OK) {
    return CanStatus::DriverError;
  }

  if (status.state == TWAI_STATE_RUNNING) {
    if (twai_stop() != ESP_OK) {
      return CanStatus::DriverError;
    }
  }
  if (twai_driver_uninstall() != ESP_OK) {
    return CanStatus::DriverError;
  }
  installed_ = false;
  started_ = false;
  pendingAlerts_ = 0;
  return CanStatus::Ok;
}

void Esp32TwaiCan::captureAlerts() {
  uint32_t alerts = 0;
  if (twai_read_alerts(&alerts, 0) == ESP_OK) {
    pendingAlerts_ |= alerts & kRequiredAlerts;
  }
}

CanStatus Esp32TwaiCan::consumeSendAlert() {
  if ((pendingAlerts_ & TWAI_ALERT_BUS_OFF) != 0) {
    pendingAlerts_ &= ~TWAI_ALERT_BUS_OFF;
    return CanStatus::BusOff;
  }
  if ((pendingAlerts_ & TWAI_ALERT_TX_FAILED) != 0) {
    pendingAlerts_ &= ~TWAI_ALERT_TX_FAILED;
    return CanStatus::TxFailed;
  }
  return CanStatus::Ok;
}

CanStatus Esp32TwaiCan::consumeReceiveAlert() {
  if ((pendingAlerts_ & (TWAI_ALERT_RX_QUEUE_FULL | TWAI_ALERT_RX_FIFO_OVERRUN)) != 0) {
    pendingAlerts_ &= ~(TWAI_ALERT_RX_QUEUE_FULL | TWAI_ALERT_RX_FIFO_OVERRUN);
    return CanStatus::RxOverflow;
  }
  if ((pendingAlerts_ & TWAI_ALERT_BUS_OFF) != 0) {
    pendingAlerts_ &= ~TWAI_ALERT_BUS_OFF;
    return CanStatus::BusOff;
  }
  return CanStatus::Ok;
}

CanStatus Esp32TwaiCan::send(const CanFrame& frame) {
  if (!started_) {
    return CanStatus::NotInitialized;
  }
  if (!frame.isValid()) {
    return CanStatus::InvalidConfig;
  }

  captureAlerts();
  const CanStatus alertStatus = consumeSendAlert();
  if (alertStatus != CanStatus::Ok) {
    return alertStatus;
  }

  twai_status_info_t status{};
  if (twai_get_status_info(&status) == ESP_OK && status.state == TWAI_STATE_BUS_OFF) {
    return CanStatus::BusOff;
  }

  twai_message_t message{};
  message.identifier = frame.id;
  message.data_length_code = frame.length;
  message.flags = frame.format == CanFrameFormat::Extended ? TWAI_MSG_FLAG_EXTD :
                                                               TWAI_MSG_FLAG_NONE;
  for (std::uint8_t index = 0; index < frame.length; ++index) {
    message.data[index] = frame.payload[index];
  }

  return twai_transmit(&message, 0) == ESP_OK ? CanStatus::Ok : CanStatus::TxFailed;
}

CanStatus Esp32TwaiCan::receive(CanFrame& frame) {
  if (!started_) {
    return CanStatus::NotInitialized;
  }

  captureAlerts();
  const CanStatus alertStatus = consumeReceiveAlert();
  if (alertStatus != CanStatus::Ok) {
    return alertStatus;
  }

  twai_status_info_t status{};
  if (twai_get_status_info(&status) == ESP_OK && status.state == TWAI_STATE_BUS_OFF) {
    return CanStatus::BusOff;
  }

  twai_message_t message{};
  const esp_err_t result = twai_receive(&message, 0);
  if (result == ESP_ERR_TIMEOUT) {
    return CanStatus::NoData;
  }
  if (result != ESP_OK) {
    return result == ESP_ERR_INVALID_STATE ? CanStatus::NotInitialized : CanStatus::NoData;
  }

  if (message.data_length_code > CanFrame::kClassicCanMaxPayload ||
      (message.flags & TWAI_MSG_FLAG_RTR) != 0) {
    return CanStatus::InvalidConfig;
  }

  frame.id = message.identifier;
  frame.format = (message.flags & TWAI_MSG_FLAG_EXTD) != 0 ? CanFrameFormat::Extended
                                                            : CanFrameFormat::Standard;
  frame.length = message.data_length_code;
  for (std::uint8_t index = 0; index < frame.length; ++index) {
    frame.payload[index] = message.data[index];
  }
  frame.timestamp = millis();
  return CanStatus::Ok;
}

}  // namespace vag_data
