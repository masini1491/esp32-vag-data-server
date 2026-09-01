#pragma once

#include "../config/hardware_config.h"
#include "../core/can_types.h"

namespace vag_data {

class CanHal {
 public:
  virtual ~CanHal() = default;

  virtual CanStatus initialize(const HardwareConfig& config) = 0;
  virtual CanStatus stop() = 0;
  // Ok means the backend accepted the frame into its transmission path;
  // it does not guarantee on-wire completion, ACK, or successful transmission.
  virtual CanStatus send(const CanFrame& frame) = 0;
  virtual CanStatus receive(CanFrame& frame) = 0;
};

}  // namespace vag_data
