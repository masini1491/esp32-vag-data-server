#pragma once

#include "can_hal.h"

namespace vag_data {

// ESP32-specific Classic CAN backend. GPIO and bitrate come from HardwareConfig.
class Esp32TwaiCan final : public CanHal {
 public:
  Esp32TwaiCan() = default;
  ~Esp32TwaiCan() override;

  Esp32TwaiCan(const Esp32TwaiCan&) = delete;
  Esp32TwaiCan& operator=(const Esp32TwaiCan&) = delete;
  Esp32TwaiCan(Esp32TwaiCan&&) = delete;
  Esp32TwaiCan& operator=(Esp32TwaiCan&&) = delete;

  CanStatus initialize(const HardwareConfig& config) override;
  CanStatus stop() override;
  // Ok means the TWAI driver accepted/queued the frame, not on-wire completion.
  CanStatus send(const CanFrame& frame) override;
  CanStatus receive(CanFrame& frame) override;

 private:
  bool installed_{false};
  bool started_{false};
};

}  // namespace vag_data
