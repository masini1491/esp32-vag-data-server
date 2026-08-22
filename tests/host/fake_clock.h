#pragma once

#include <cstdint>

#include "../../src/core/clock.h"

namespace vag_data::test {

class FakeClock final : public Clock {
 public:
  std::uint64_t nowMs() const override { return nowMs_; }

  void setMs(std::uint64_t value) { nowMs_ = value; }

  void advanceMs(std::uint64_t delta) { nowMs_ += delta; }

  void reset() { nowMs_ = 0; }

 private:
  std::uint64_t nowMs_{0};
};

}  // namespace vag_data::test
