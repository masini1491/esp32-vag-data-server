#pragma once

#include <cstdint>

namespace vag_data {

// Monotonic protocol/test time. Implementations own the time source.
class Clock {
 public:
  virtual ~Clock() = default;
  virtual std::uint64_t nowMs() const = 0;
};

}  // namespace vag_data
