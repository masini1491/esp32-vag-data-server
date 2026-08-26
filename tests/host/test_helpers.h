#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <iostream>

#include "../../src/core/can_types.h"

namespace vag_data::test {

inline void expect(bool condition, const char* expression) {
  if (!condition) {
    std::cerr << "FAIL: " << expression << '\n';
    std::exit(EXIT_FAILURE);
  }
}

inline CanFrame makeFrame(CanId id, std::initializer_list<std::uint8_t> bytes,
                          CanFrameFormat format = CanFrameFormat::Standard) {
  CanFrame frame;
  frame.id = id;
  frame.format = format;
  frame.length = static_cast<std::uint8_t>(bytes.size());
  const auto copyCount = std::min(bytes.size(), frame.payload.size());
  std::copy_n(bytes.begin(), copyCount, frame.payload.begin());
  return frame;
}

inline bool sameFrame(const CanFrame& left, const CanFrame& right) {
  if (left.id != right.id || left.format != right.format ||
      left.length != right.length || left.timestamp != right.timestamp) {
    return false;
  }
  if (!left.isValid() || !right.isValid()) {
    return false;
  }
  return std::equal(left.payload.begin(), left.payload.begin() + left.length,
                    right.payload.begin());
}

}  // namespace vag_data::test

#define EXPECT_TRUE(value) ::vag_data::test::expect((value), #value)
