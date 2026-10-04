#pragma once

#include <cstring>
#include <type_traits>

#include "../../src/profiles/active_vehicle_profile.h"
#include "../../src/vag/profile_set.h"
#include "vehicle_data_store_tests.h"

namespace vag_data::test {

inline void runProfileSelectionTests() {
  static_assert(!std::is_convertible_v<ProfileIdentity, std::uint32_t>);
  static_assert(!std::is_convertible_v<std::uint32_t, ProfileIdentity>);
  static_assert(vag::kProfileSet.size() == 1);
  static_assert(vag::kProfileSet[0].validation ==
                vag::ProfileValidationState::Pending);

  const auto admitted = vag::kProfileSet[0].identity;
  const ProfileIdentity outside{999};
  ActiveVehicleProfile<1> selection(vag::kAdmittedProfileIdentities);
  EXPECT_TRUE(selection.state() == ProfileSelectionState::Unknown);
  EXPECT_TRUE(!selection.activeIdentity().has_value());
  EXPECT_TRUE(!selection.selectManually(outside));
  EXPECT_TRUE(selection.state() == ProfileSelectionState::Unknown);
  EXPECT_TRUE(!selection.activeIdentity().has_value());

  VehicleDataStore<2> store;
  const auto sample = makeNumericSample("test.signal", 17.0,
                                       VehicleSource::Unknown,
                                       VehicleQuality::ValidCurrent, 42);
  EXPECT_TRUE(store.upsert(sample) == VehicleDataStoreUpsertResult::Inserted);

  EXPECT_TRUE(selection.selectManually(admitted));
  EXPECT_TRUE(selection.state() == ProfileSelectionState::Selected);
  EXPECT_TRUE(selection.activeIdentity() == admitted);
  EXPECT_TRUE(!selection.selectManually(outside));
  EXPECT_TRUE(selection.state() == ProfileSelectionState::Selected);
  EXPECT_TRUE(selection.activeIdentity() == admitted);
  EXPECT_TRUE(selection.selectManually(admitted));
  EXPECT_TRUE(selection.activeIdentity() == admitted);

  selection.setUnknown();
  EXPECT_TRUE(selection.state() == ProfileSelectionState::Unknown);
  EXPECT_TRUE(!selection.activeIdentity().has_value());
  EXPECT_TRUE(selection.selectManually(admitted));
  selection.setAmbiguous();
  EXPECT_TRUE(selection.state() == ProfileSelectionState::Ambiguous);
  EXPECT_TRUE(!selection.activeIdentity().has_value());
  EXPECT_TRUE(!selection.selectManually(outside));
  EXPECT_TRUE(selection.state() == ProfileSelectionState::Ambiguous);
  EXPECT_TRUE(selection.selectManually(admitted));
  selection.requireManualSelection();
  EXPECT_TRUE(selection.state() == ProfileSelectionState::ManualSelectionRequired);
  EXPECT_TRUE(!selection.activeIdentity().has_value());
  EXPECT_TRUE(!selection.selectManually(outside));
  EXPECT_TRUE(selection.state() == ProfileSelectionState::ManualSelectionRequired);

  EXPECT_TRUE(vag::kProfileSet[0].validation == vag::ProfileValidationState::Pending);
  EXPECT_TRUE(vag::kProfileSet[0].validation !=
              vag::ProfileValidationState::VehicleConfirmed);
  EXPECT_TRUE(std::strcmp(vag::kProfileSet[0].modelReference, "Kamiq_NW4") == 0);
  EXPECT_TRUE(store.size() == 1);
  NormalizedVehicleSample retained;
  EXPECT_TRUE(store.lookup(sample.signalId(), retained));
  EXPECT_TRUE(retained.value().numericValue() == 17.0);
  EXPECT_TRUE(retained.timestampMs() == 42);
  EXPECT_TRUE(retained.availability() == sample.availability());
  EXPECT_TRUE(retained.quality() == sample.quality());
  EXPECT_TRUE(retained.source() == sample.source());

  // Even an empty admitted set cannot acquire an active identity.
  ActiveVehicleProfile<0> empty(std::array<ProfileIdentity, 0>{});
  EXPECT_TRUE(!empty.selectManually(admitted));
  EXPECT_TRUE(!empty.activeIdentity().has_value());
}

}  // namespace vag_data::test
