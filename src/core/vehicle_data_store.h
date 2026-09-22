#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "vehicle_data.h"

namespace vag_data {

enum class VehicleDataStoreUpsertResult : std::uint8_t {
  Inserted,
  Updated,
  IgnoredOlder,
  Full,
};

enum class VehicleDataStoreSnapshotResult : std::uint8_t {
  Complete,
  InsufficientCapacity,
};

template <std::size_t Capacity>
class VehicleDataStore {
 public:
  static_assert(Capacity > 0, "VehicleDataStore capacity must be positive");

  VehicleDataStoreUpsertResult upsert(
      const NormalizedVehicleSample& incoming) {
    for (std::size_t index = 0; index < size_; ++index) {
      if (!(samples_[index].signalId() == incoming.signalId())) {
        continue;
      }
      if (incoming.timestampMs() < samples_[index].timestampMs()) {
        return VehicleDataStoreUpsertResult::IgnoredOlder;
      }
      samples_[index] = incoming;
      return VehicleDataStoreUpsertResult::Updated;
    }

    if (size_ == Capacity) {
      return VehicleDataStoreUpsertResult::Full;
    }
    samples_[size_] = incoming;
    ++size_;
    return VehicleDataStoreUpsertResult::Inserted;
  }

  bool lookup(const NormalizedVehicleSample::SignalId& signalId,
              NormalizedVehicleSample& result) const {
    for (std::size_t index = 0; index < size_; ++index) {
      if (samples_[index].signalId() == signalId) {
        result = samples_[index];
        return true;
      }
    }
    return false;
  }

  VehicleDataStoreSnapshotResult snapshot(NormalizedVehicleSample* output,
                                          std::size_t outputCapacity,
                                          std::size_t& resultCount) const {
    resultCount = size_;
    if (outputCapacity < size_ || (size_ != 0 && output == nullptr)) {
      return VehicleDataStoreSnapshotResult::InsufficientCapacity;
    }
    for (std::size_t index = 0; index < size_; ++index) {
      output[index] = samples_[index];
    }
    return VehicleDataStoreSnapshotResult::Complete;
  }

  std::size_t size() const { return size_; }
  static constexpr std::size_t capacity() { return Capacity; }

 private:
  std::array<NormalizedVehicleSample, Capacity> samples_{};
  std::size_t size_{0};
};

}  // namespace vag_data
