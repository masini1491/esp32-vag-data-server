#pragma once

#include <cstring>

#include "../../src/core/vehicle_data_store.h"
#include "test_helpers.h"

namespace vag_data::test {

inline NormalizedVehicleSample makeNumericSample(const char* signalId,
                                                 double value,
                                                 VehicleSource source,
                                                 VehicleQuality quality,
                                                 std::uint64_t timestampMs) {
  NormalizedVehicleSample sample;
  EXPECT_TRUE(NormalizedVehicleSample::tryNumeric(
      signalId, value, "unit", source, quality, timestampMs, sample));
  return sample;
}

inline NormalizedVehicleSample makeStatusSample(
    const char* signalId, VehicleAvailability availability,
    std::uint64_t timestampMs) {
  NormalizedVehicleSample sample;
  EXPECT_TRUE(NormalizedVehicleSample::tryStatus(
      signalId, availability, "unit", VehicleSource::Unknown,
      VehicleQuality::InvalidOrUnknown, timestampMs, sample));
  return sample;
}

inline void runVehicleDataStoreTests() {
  VehicleDataStore<2> store;
  const auto initial = makeNumericSample("engine.speed", 100.0,
                                         VehicleSource::Obd,
                                         VehicleQuality::ValidCurrent,
                                         0x100000000ULL);
  EXPECT_TRUE(store.upsert(initial) == VehicleDataStoreUpsertResult::Inserted);
  EXPECT_TRUE(store.size() == 1);

  NormalizedVehicleSample lookedUp;
  EXPECT_TRUE(store.lookup(NormalizedVehicleSample::SignalId("engine.speed"),
                           lookedUp));
  EXPECT_TRUE(lookedUp.value().numericValue() == 100.0);
  EXPECT_TRUE(lookedUp.timestampMs() == 0x100000000ULL);
  lookedUp = makeNumericSample("engine.speed", 999.0, VehicleSource::Derived,
                               VehicleQuality::ValidCurrent, 1);
  EXPECT_TRUE(store.lookup(NormalizedVehicleSample::SignalId("engine.speed"),
                           lookedUp));
  EXPECT_TRUE(lookedUp.value().numericValue() == 100.0);

  const auto newer = makeNumericSample("engine.speed", 200.0,
                                       VehicleSource::Uds,
                                       VehicleQuality::Stale,
                                       0x100000001ULL);
  EXPECT_TRUE(store.upsert(newer) == VehicleDataStoreUpsertResult::Updated);
  EXPECT_TRUE(store.size() == 1);
  EXPECT_TRUE(store.lookup(NormalizedVehicleSample::SignalId("engine.speed"),
                           lookedUp));
  EXPECT_TRUE(lookedUp.value().numericValue() == 200.0);
  EXPECT_TRUE(lookedUp.source() == VehicleSource::Uds);
  EXPECT_TRUE(lookedUp.quality() == VehicleQuality::Stale);
  EXPECT_TRUE(lookedUp.hasValue());

  const auto equalTimestamp = makeNumericSample("engine.speed", 300.0,
                                                VehicleSource::Derived,
                                                VehicleQuality::ValidCurrent,
                                                0x100000001ULL);
  EXPECT_TRUE(store.upsert(equalTimestamp) == VehicleDataStoreUpsertResult::Updated);
  EXPECT_TRUE(store.lookup(NormalizedVehicleSample::SignalId("engine.speed"),
                           lookedUp));
  EXPECT_TRUE(lookedUp.value().numericValue() == 300.0);
  EXPECT_TRUE(lookedUp.source() == VehicleSource::Derived);
  EXPECT_TRUE(lookedUp.isCurrent());

  const auto older = makeNumericSample("engine.speed", 50.0,
                                       VehicleSource::Obd,
                                       VehicleQuality::ValidCurrent,
                                       0xFFFFFFFFULL);
  EXPECT_TRUE(store.upsert(older) == VehicleDataStoreUpsertResult::IgnoredOlder);
  EXPECT_TRUE(store.lookup(NormalizedVehicleSample::SignalId("engine.speed"),
                           lookedUp));
  EXPECT_TRUE(lookedUp.value().numericValue() == 300.0);

  for (const auto availability : {VehicleAvailability::Unavailable,
                                  VehicleAvailability::Pending,
                                  VehicleAvailability::Unsupported,
                                  VehicleAvailability::Unknown}) {
    const auto status = makeStatusSample("engine.speed", availability,
                                         lookedUp.timestampMs() + 1);
    EXPECT_TRUE(store.upsert(status) == VehicleDataStoreUpsertResult::Updated);
    EXPECT_TRUE(store.lookup(NormalizedVehicleSample::SignalId("engine.speed"),
                             lookedUp));
    EXPECT_TRUE(lookedUp.availability() == availability);
    EXPECT_TRUE(!lookedUp.hasValue());
  }

  const auto stale = makeNumericSample("engine.speed", 250.0,
                                       VehicleSource::PassiveCan,
                                       VehicleQuality::Stale,
                                       lookedUp.timestampMs() + 1);
  EXPECT_TRUE(store.upsert(stale) == VehicleDataStoreUpsertResult::Updated);
  EXPECT_TRUE(store.lookup(NormalizedVehicleSample::SignalId("engine.speed"),
                           lookedUp));
  EXPECT_TRUE(lookedUp.hasValue());
  EXPECT_TRUE(!lookedUp.isCurrent());

  const auto voltage = makeNumericSample("battery.voltage", 12.3,
                                         VehicleSource::Obd,
                                         VehicleQuality::ValidCurrent, 1);
  EXPECT_TRUE(store.upsert(voltage) == VehicleDataStoreUpsertResult::Inserted);
  const auto rejected = makeNumericSample("coolant.temperature", 90.0,
                                          VehicleSource::Obd,
                                          VehicleQuality::ValidCurrent, 1);
  EXPECT_TRUE(store.upsert(rejected) == VehicleDataStoreUpsertResult::Full);
  EXPECT_TRUE(store.size() == 2);
  EXPECT_TRUE(!store.lookup(NormalizedVehicleSample::SignalId("coolant.temperature"),
                            lookedUp));

  const auto fullUpdate = makeNumericSample("battery.voltage", 12.5,
                                            VehicleSource::Uds,
                                            VehicleQuality::ValidCurrent, 2);
  EXPECT_TRUE(store.upsert(fullUpdate) == VehicleDataStoreUpsertResult::Updated);
  EXPECT_TRUE(store.lookup(NormalizedVehicleSample::SignalId("battery.voltage"),
                           lookedUp));
  EXPECT_TRUE(lookedUp.value().numericValue() == 12.5);

  NormalizedVehicleSample insufficient[1];
  std::size_t snapshotCount = 0;
  EXPECT_TRUE(store.snapshot(insufficient, 1, snapshotCount) ==
              VehicleDataStoreSnapshotResult::InsufficientCapacity);
  EXPECT_TRUE(snapshotCount == 2);

  NormalizedVehicleSample snapshot[2];
  EXPECT_TRUE(store.snapshot(snapshot, 2, snapshotCount) ==
              VehicleDataStoreSnapshotResult::Complete);
  EXPECT_TRUE(snapshotCount == 2);
  EXPECT_TRUE(std::strcmp(snapshot[0].signalId().c_str(), "engine.speed") == 0);
  EXPECT_TRUE(std::strcmp(snapshot[1].signalId().c_str(), "battery.voltage") == 0);
  snapshot[0] = makeNumericSample("engine.speed", 1.0, VehicleSource::Unknown,
                                  VehicleQuality::ValidCurrent, 3);
  EXPECT_TRUE(store.lookup(NormalizedVehicleSample::SignalId("engine.speed"),
                           lookedUp));
  EXPECT_TRUE(lookedUp.value().numericValue() == 250.0);
}

}  // namespace vag_data::test
