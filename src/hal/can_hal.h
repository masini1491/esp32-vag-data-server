#pragma once

#include "../config/hardware_config.h"
#include "../core/can_types.h"

namespace vag_data {

class CanHal {
 public:
  virtual ~CanHal() = default;

  virtual CanStatus initialize(const HardwareConfig& config) = 0;
  virtual CanStatus stop() = 0;
  virtual CanStatus send(const CanFrame& frame) = 0;
  virtual CanStatus receive(CanFrame& frame) = 0;
};

}  // namespace vag_data
