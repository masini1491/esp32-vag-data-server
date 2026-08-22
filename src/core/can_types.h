#pragma once

#include <array>
#include <cstdint>

namespace vag_data {

enum class CanFrameFormat : std::uint8_t {
  Standard,
  Extended,
};

using CanId = std::uint32_t;

enum class CanStatus : std::uint8_t {
  Ok,
  NotInitialized,
  InvalidConfig,
  NoData,
  TxFailed,
};

struct CanFrame {
  static constexpr std::uint8_t kClassicCanMaxPayload = 8;

  CanId id{0};
  CanFrameFormat format{CanFrameFormat::Standard};
  std::uint8_t length{0};
  std::array<std::uint8_t, kClassicCanMaxPayload> payload{};
  std::uint64_t timestamp{0};

  bool isValid() const {
    return length <= kClassicCanMaxPayload;
  }
};

}  // namespace vag_data
