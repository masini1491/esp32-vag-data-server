#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "../core/vehicle_data_store.h"
#include "../profiles/active_vehicle_profile.h"
#include "../profiles/signal_registry.h"

namespace vag_data {

enum class VehicleDataReadStatus : std::uint8_t {
  Complete,
  InsufficientCapacity,
};

struct VehicleDataReadResult {
  VehicleDataReadStatus status;
  std::size_t requiredSampleCapacity;
  std::size_t requiredCapabilityCapacity;
};

struct VehicleDataReadSnapshot {
  ProfileSelectionState selectionState{ProfileSelectionState::Unknown};
  std::optional<ProfileIdentity> activeIdentity;
  bool hasCapabilityRegistry{false};
  std::size_t capabilityCount{0};
  std::size_t sampleCount{0};
};

// Synchronous read projection; referenced owners must outlive this adapter.
// A registry is explicitly associated with one profile, never inferred from data.
template <std::size_t ProfileCount, std::size_t StoreCapacity,
          std::size_t CapabilityCount = 0>
class VehicleDataReadModel {
 public:
  VehicleDataReadModel(const ActiveVehicleProfile<ProfileCount>& profile,
                       const VehicleDataStore<StoreCapacity>& store)
      : profile_(profile), store_(store) {}

  VehicleDataReadModel(const ActiveVehicleProfile<ProfileCount>& profile,
                       const VehicleDataStore<StoreCapacity>& store,
                       ProfileIdentity registryProfile,
                       const FixedSignalRegistry<CapabilityCount>& registry)
      : profile_(profile), store_(store), registryProfile_(registryProfile),
        registry_(&registry) {}

  // On failure, all caller buffers and metadata remain unchanged. Required
  // capacities are returned separately; only Complete publishes a new snapshot.
  VehicleDataReadResult snapshot(
      NormalizedVehicleSample* samples, std::size_t sampleCapacity,
      NormalizedSignalDescriptor* capabilities, std::size_t capabilityCapacity,
      VehicleDataReadSnapshot& metadata) const {
    VehicleDataReadSnapshot next;
    next.selectionState = profile_.state();
    if (next.selectionState == ProfileSelectionState::Selected) {
      next.activeIdentity = profile_.activeIdentity();
    }
    next.hasCapabilityRegistry = registry_ != nullptr && next.activeIdentity &&
                                 next.activeIdentity == registryProfile_;
    next.capabilityCount = next.hasCapabilityRegistry ? registry_->size() : 0;

    if (capabilityCapacity < next.capabilityCount ||
        (next.capabilityCount != 0 && capabilities == nullptr)) {
      return {VehicleDataReadStatus::InsufficientCapacity, store_.size(),
              next.capabilityCount};
    }
    if (store_.snapshot(samples, sampleCapacity, next.sampleCount) !=
        VehicleDataStoreSnapshotResult::Complete) {
      return {VehicleDataReadStatus::InsufficientCapacity, next.sampleCount,
              next.capabilityCount};
    }

    // No fallible operation follows the Store's successful copy.
    for (std::size_t index = 0; index < next.capabilityCount; ++index) {
      capabilities[index] = registry_->descriptors()[index];
    }
    metadata = next;
    return {VehicleDataReadStatus::Complete, next.sampleCount,
            next.capabilityCount};
  }

 private:
  const ActiveVehicleProfile<ProfileCount>& profile_;
  const VehicleDataStore<StoreCapacity>& store_;
  std::optional<ProfileIdentity> registryProfile_;
  const FixedSignalRegistry<CapabilityCount>* registry_{nullptr};
};

}  // namespace vag_data
