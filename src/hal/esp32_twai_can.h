#pragma once

#include "can_hal.h"

namespace vag_data {

// ESP32-specific Classic CAN backend. GPIO and bitrate come from HardwareConfig.
class Esp32TwaiCan final : public CanHal {
 public:
  ~Esp32TwaiCan() override;

  CanStatus initialize(const HardwareConfig& config) override;
  CanStatus stop() override;
  CanStatus send(const CanFrame& frame) override;
  CanStatus receive(CanFrame& frame) override;

 private:
  bool installed_{false};
  bool started_{false};
};

}  // namespace vag_data
