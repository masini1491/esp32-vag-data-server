#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "../core/vehicle_data.h"

namespace vag_data {

// Profile-authoritative support knowledge, separate from sample availability.
enum class CapabilitySupport : std::uint8_t {
  Supported,
  Unsupported,
  Pending,
  Unknown,
};

struct NormalizedSignalDescriptor {
  NormalizedVehicleSample::SignalId identity;
  VehicleValueType expectedValueType;
  NormalizedVehicleSample::Unit unit;
  CapabilitySupport support;
};

// Immutable owned metadata. Lookup never observes diagnostic results or samples.
template <std::size_t Count>
class FixedSignalRegistry {
 public:
  explicit FixedSignalRegistry(
      const std::array<NormalizedSignalDescriptor, Count>& descriptors)
      : descriptors_(descriptors) {}

  constexpr std::size_t size() const { return Count; }

  // Not registered is not a determination of Unsupported capability.
  const NormalizedSignalDescriptor* lookup(
      const NormalizedVehicleSample::SignalId& identity) const {
    for (const auto& descriptor : descriptors_) {
      if (descriptor.identity == identity) {
        return &descriptor;
      }
    }
    return nullptr;
  }

 private:
  const std::array<NormalizedSignalDescriptor, Count> descriptors_;
};

}  // namespace vag_data
