#pragma once

#include <cstdint>

namespace vag_data {

// A profile describes board capability/defaults without selecting a concrete board.
struct BoardProfile {
  const char* name{"generic-development"};
  std::int16_t canTxGpio{-1};
  std::int16_t canRxGpio{-1};
  std::uint32_t canBitrate{0};
};

}  // namespace vag_data
