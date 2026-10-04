#pragma once

#include <array>
#include <type_traits>

#include "../../src/vag/kamiq_nw4_capabilities.h"
#include "vehicle_data_store_tests.h"

namespace vag_data::test {

inline void runSignalRegistryTests() {
  using SignalId = NormalizedVehicleSample::SignalId;
  static_assert(!std::is_same_v<CapabilitySupport, VehicleAvailability>);
  static_assert(!std::is_convertible_v<VehicleAvailability, CapabilitySupport>);
  static_assert(std::is_same_v<decltype(NormalizedSignalDescriptor::identity),
                               SignalId>);
  static_assert(std::is_same_v<decltype(NormalizedSignalDescriptor::unit),
                               NormalizedVehicleSample::Unit>);
  static_assert(std::is_same_v<decltype(vag::kKamiqNw4Capabilities.lookup(
                                   SignalId{})),
                               const NormalizedSignalDescriptor*>);

  const auto& registry = vag::kKamiqNw4Capabilities;
  EXPECT_TRUE(registry.size() == 4);
  const std::array<const char*, 4> identities{{
      "vehicle.speed", "vehicle.rpm", "vehicle.coolantTemp", "vehicle.voltage"}};
  const std::array<const char*, 4> units{{"km/h", "rpm", "degC", "V"}};
  for (std::size_t index = 0; index < identities.size(); ++index) {
    const auto* descriptor = registry.lookup(SignalId(identities[index]));
    EXPECT_TRUE(descriptor != nullptr);
    EXPECT_TRUE(descriptor->identity == SignalId(identities[index]));
    EXPECT_TRUE(descriptor->expectedValueType == VehicleValueType::Numeric);
    EXPECT_TRUE(descriptor->unit == NormalizedVehicleSample::Unit(units[index]));
    EXPECT_TRUE(descriptor->support == CapabilitySupport::Pending);
    EXPECT_TRUE(registry.lookup(SignalId(identities[index])) == descriptor);
  }
  EXPECT_TRUE(registry.lookup(SignalId("vehicle.unknown")) == nullptr);
  EXPECT_TRUE(registry.lookup(SignalId{}) == nullptr);
  EXPECT_TRUE(vag::signalRegistryFor(ProfileIdentity{999}) == nullptr);

  ActiveVehicleProfile<1> selection(vag::kAdmittedProfileIdentities);
  EXPECT_TRUE(selection.selectManually(vag::kProfileSet[0].identity));
  EXPECT_TRUE(vag::signalRegistryFor(*selection.activeIdentity()) == &registry);
  EXPECT_TRUE(vag::kProfileSet[0].validation == vag::ProfileValidationState::Pending);

  // Registry queries and selection have no sample/store mutation dependency.
  VehicleDataStore<2> store;
  auto sample = makeStatusSample("vehicle.speed", VehicleAvailability::Unavailable, 42);
  EXPECT_TRUE(store.upsert(sample) == VehicleDataStoreUpsertResult::Inserted);
  for (const auto* identity : identities) {
    EXPECT_TRUE(registry.lookup(SignalId(identity))->support == CapabilitySupport::Pending);
  }
  EXPECT_TRUE(store.size() == 1);
  NormalizedVehicleSample retained;
  EXPECT_TRUE(store.lookup(sample.signalId(), retained));
  EXPECT_TRUE(retained.availability() == VehicleAvailability::Unavailable);
  EXPECT_TRUE(retained.valueType() == VehicleValueType::None);
  EXPECT_TRUE(retained.timestampMs() == 42);
  EXPECT_TRUE(sample.availability() == VehicleAvailability::Unavailable);
  EXPECT_TRUE(registry.lookup(sample.signalId())->support == CapabilitySupport::Pending);
  selection.setUnknown();
  EXPECT_TRUE(registry.lookup(sample.signalId())->support == CapabilitySupport::Pending);

  const FixedSignalRegistry<0> empty(std::array<NormalizedSignalDescriptor, 0>{});
  EXPECT_TRUE(empty.size() == 0);
  EXPECT_TRUE(empty.lookup(SignalId("vehicle.speed")) == nullptr);
}

}  // namespace vag_data::test
