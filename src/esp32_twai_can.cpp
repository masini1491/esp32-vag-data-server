#include "hal/esp32_twai_can.h"

#include <Arduino.h>
#include "driver/twai.h"

namespace vag_data {
namespace {

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
    stop();
  }

  twai_general_config_t general = TWAI_GENERAL_CONFIG_DEFAULT(
      static_cast<gpio_num_t>(config.canTxGpio),
      static_cast<gpio_num_t>(config.canRxGpio), TWAI_MODE_NORMAL);
  twai_filter_config_t filter = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  if (twai_driver_install(&general, &timing, &filter) != ESP_OK) {
    return CanStatus::InvalidConfig;
  }
  installed_ = true;

  if (twai_start() != ESP_OK) {
    twai_driver_uninstall();
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

  CanStatus result = CanStatus::Ok;
  if (started_ && twai_stop() != ESP_OK) {
    result = CanStatus::NotInitialized;
  }
  started_ = false;
  if (twai_driver_uninstall() != ESP_OK) {
    result = CanStatus::NotInitialized;
  }
  installed_ = false;
  return result;
}

CanStatus Esp32TwaiCan::send(const CanFrame& frame) {
  if (!started_) {
    return CanStatus::NotInitialized;
  }
  if (!frame.isValid()) {
    return CanStatus::InvalidConfig;
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

  twai_message_t message{};
  const esp_err_t result = twai_receive(&message, 0);
  if (result == ESP_ERR_TIMEOUT) {
    return CanStatus::NoData;
  }
  if (result != ESP_OK) {
    return result == ESP_ERR_INVALID_STATE ? CanStatus::NotInitialized : CanStatus::NoData;
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
