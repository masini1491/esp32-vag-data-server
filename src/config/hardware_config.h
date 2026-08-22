#pragma once

#include <cstdint>

#include "../board/board_profile.h"

namespace vag_data {

// Runtime-resolved hardware settings. -1/0 means not configured.
struct HardwareConfig {
  std::int16_t canTxGpio{-1};
  std::int16_t canRxGpio{-1};
  std::uint32_t canBitrate{0};

  static HardwareConfig fromBoardProfile(const BoardProfile& profile) {
    return {profile.canTxGpio, profile.canRxGpio, profile.canBitrate};
  }
};

}  // namespace vag_data
