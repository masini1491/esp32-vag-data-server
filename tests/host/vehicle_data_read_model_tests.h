#pragma once

#include <array>
#include <type_traits>

#include "../../src/application/vehicle_data_read_model.h"
#include "../../src/vag/kamiq_nw4_capabilities.h"
#include "vehicle_data_store_tests.h"

namespace vag_data::test {

inline void expectReadSampleEqual(const NormalizedVehicleSample& actual,
                                  const NormalizedVehicleSample& expected) {
  EXPECT_TRUE(actual.signalId() == expected.signalId());
  EXPECT_TRUE(actual.unit() == expected.unit());
  EXPECT_TRUE(actual.source() == expected.source());
  EXPECT_TRUE(actual.quality() == expected.quality());
  EXPECT_TRUE(actual.availability() == expected.availability());
  EXPECT_TRUE(actual.timestampMs() == expected.timestampMs());
  EXPECT_TRUE(actual.valueType() == expected.valueType());
  EXPECT_TRUE(actual.value().numericValue() == expected.value().numericValue());
  EXPECT_TRUE(actual.value().booleanValue() == expected.value().booleanValue());
  EXPECT_TRUE(actual.value().textValue() == expected.value().textValue());
}

inline void expectReadDescriptorEqual(const NormalizedSignalDescriptor& actual,
                                      const NormalizedSignalDescriptor& expected) {
  EXPECT_TRUE(actual.identity == expected.identity);
  EXPECT_TRUE(actual.expectedValueType == expected.expectedValueType);
  EXPECT_TRUE(actual.unit == expected.unit);
  EXPECT_TRUE(actual.support == expected.support);
}

inline void testReadModelProfileAndCapabilities() {
  static_assert(std::is_same_v<decltype(vag::kKamiqNw4Capabilities.descriptors()),
                              const std::array<NormalizedSignalDescriptor, 4>&>);
  const auto kamiq = vag::kProfileSet[0].identity;
  const ProfileIdentity unregistered{999};
  ActiveVehicleProfile<2> profile({kamiq, unregistered});
  VehicleDataStore<2> store;
  const VehicleDataReadModel<2, 2, 4> reader(
      profile, store, kamiq, vag::kKamiqNw4Capabilities);
  VehicleDataReadSnapshot metadata;
  std::array<NormalizedSignalDescriptor, 4> capabilities{};

  for (const auto state : {ProfileSelectionState::Unknown,
                           ProfileSelectionState::Ambiguous,
                           ProfileSelectionState::ManualSelectionRequired}) {
    EXPECT_TRUE(profile.selectManually(kamiq));
    metadata.activeIdentity = kamiq;  // Prove an unresolved read clears old metadata.
    switch (state) {
      case ProfileSelectionState::Unknown: profile.setUnknown(); break;
      case ProfileSelectionState::Ambiguous: profile.setAmbiguous(); break;
      case ProfileSelectionState::ManualSelectionRequired:
        profile.requireManualSelection(); break;
      case ProfileSelectionState::Selected: break;
    }
    EXPECT_TRUE(reader.snapshot(nullptr, 0, nullptr, 0, metadata).status ==
                VehicleDataReadStatus::Complete);
    EXPECT_TRUE(metadata.selectionState == state);
    EXPECT_TRUE(!metadata.activeIdentity);
    EXPECT_TRUE(!metadata.hasCapabilityRegistry);
    EXPECT_TRUE(metadata.capabilityCount == 0 && metadata.sampleCount == 0);
    EXPECT_TRUE(profile.state() == state && !profile.activeIdentity());
  }

  EXPECT_TRUE(profile.selectManually(kamiq));
  EXPECT_TRUE(reader.snapshot(nullptr, 0, capabilities.data(), capabilities.size(),
                              metadata).status == VehicleDataReadStatus::Complete);
  EXPECT_TRUE(metadata.selectionState == ProfileSelectionState::Selected);
  EXPECT_TRUE(metadata.activeIdentity == kamiq);
  EXPECT_TRUE(metadata.hasCapabilityRegistry && metadata.capabilityCount == 4);
  const std::array<const char*, 4> identities{
      "vehicle.speed", "vehicle.rpm", "vehicle.coolantTemp", "vehicle.voltage"};
  for (std::size_t index = 0; index < capabilities.size(); ++index) {
    expectReadDescriptorEqual(capabilities[index],
                              vag::kKamiqNw4Capabilities.descriptors()[index]);
    EXPECT_TRUE(capabilities[index].identity ==
                NormalizedVehicleSample::SignalId(identities[index]));
    EXPECT_TRUE(capabilities[index].support == CapabilitySupport::Pending);
  }

  EXPECT_TRUE(profile.selectManually(unregistered));
  EXPECT_TRUE(reader.snapshot(nullptr, 0, nullptr, 0, metadata).status ==
              VehicleDataReadStatus::Complete);
  EXPECT_TRUE(metadata.activeIdentity == unregistered);
  EXPECT_TRUE(!metadata.hasCapabilityRegistry && metadata.capabilityCount == 0);
  const VehicleDataReadModel<2, 2> noRegistry(profile, store);
  EXPECT_TRUE(profile.selectManually(kamiq));
  EXPECT_TRUE(noRegistry.snapshot(nullptr, 0, nullptr, 0, metadata).status ==
              VehicleDataReadStatus::Complete);
  EXPECT_TRUE(metadata.activeIdentity == kamiq);
  EXPECT_TRUE(!metadata.hasCapabilityRegistry && metadata.capabilityCount == 0);

  const FixedSignalRegistry<0> emptyRegistry({});
  const VehicleDataReadModel<2, 2> emptyReader(profile, store, kamiq, emptyRegistry);
  EXPECT_TRUE(emptyReader.snapshot(nullptr, 0, nullptr, 0, metadata).status ==
              VehicleDataReadStatus::Complete);
  EXPECT_TRUE(metadata.hasCapabilityRegistry && metadata.capabilityCount == 0);
}

inline void testReadModelPreservesSamplesAndOwners() {
  const auto kamiq = vag::kProfileSet[0].identity;
  ActiveVehicleProfile<1> profile(vag::kAdmittedProfileIdentities);
  EXPECT_TRUE(profile.selectManually(kamiq));
  VehicleDataStore<8> store;
  const VehicleDataReadModel<1, 8, 4> reader(
      profile, store, kamiq, vag::kKamiqNw4Capabilities);
  std::array<NormalizedVehicleSample, 8> expected{};
  EXPECT_TRUE(NormalizedVehicleSample::tryNumeric(
      "vehicle.speed", 12.5, "km/h", VehicleSource::Obd,
      VehicleQuality::ValidCurrent, 0x100000000ULL, expected[0]));
  EXPECT_TRUE(NormalizedVehicleSample::tryBoolean(
      "flag", true, "bool", VehicleSource::Derived,
      VehicleQuality::Stale, 7, expected[1]));
  EXPECT_TRUE(NormalizedVehicleSample::tryText(
      "identity", "ABC", "text", VehicleSource::Uds,
      VehicleQuality::InvalidOrUnknown, 9, expected[2]));
  std::size_t index = 3;
  for (const auto availability : {VehicleAvailability::Unsupported,
                                  VehicleAvailability::Unavailable,
                                  VehicleAvailability::Pending,
                                  VehicleAvailability::Unknown}) {
    const char* ids[] = {"unsupported", "unavailable", "pending", "unknown"};
    EXPECT_TRUE(NormalizedVehicleSample::tryStatus(
        ids[index - 3], availability, "unit", VehicleSource::Unknown,
        VehicleQuality::InvalidOrUnknown, index, expected[index]));
    ++index;
  }
  EXPECT_TRUE(NormalizedVehicleSample::tryNumeric(
      "invalidNumeric", -4.0, "V", VehicleSource::PassiveCan,
      VehicleQuality::InvalidOrUnknown, 11, expected[7]));
  for (const auto& sample : expected) {
    EXPECT_TRUE(store.upsert(sample) == VehicleDataStoreUpsertResult::Inserted);
  }
  const auto originalDescriptors = vag::kKamiqNw4Capabilities.descriptors();
  std::array<NormalizedVehicleSample, 8> samples{};
  std::array<NormalizedSignalDescriptor, 4> capabilities{};
  VehicleDataReadSnapshot metadata;
  for (int read = 0; read < 3; ++read) {
    const auto result = reader.snapshot(samples.data(), samples.size(),
                                        capabilities.data(), capabilities.size(), metadata);
    EXPECT_TRUE(result.status == VehicleDataReadStatus::Complete);
    EXPECT_TRUE(result.requiredSampleCapacity == 8 && result.requiredCapabilityCapacity == 4);
    EXPECT_TRUE(metadata.sampleCount == 8 && metadata.capabilityCount == 4);
    EXPECT_TRUE(metadata.activeIdentity == kamiq);
    for (index = 0; index < expected.size(); ++index) {
      expectReadSampleEqual(samples[index], expected[index]);
      NormalizedVehicleSample retained;
      EXPECT_TRUE(store.lookup(expected[index].signalId(), retained));
      expectReadSampleEqual(retained, expected[index]);
    }
    for (index = 0; index < capabilities.size(); ++index) {
      expectReadDescriptorEqual(capabilities[index], originalDescriptors[index]);
      expectReadDescriptorEqual(vag::kKamiqNw4Capabilities.descriptors()[index],
                                originalDescriptors[index]);
    }
    EXPECT_TRUE(store.size() == 8);
    EXPECT_TRUE(profile.state() == ProfileSelectionState::Selected);
    EXPECT_TRUE(profile.activeIdentity() == kamiq);
    // Caller copies do not mutate canonical owners or later reads.
    samples[0] = NormalizedVehicleSample{};
    capabilities[0].support = CapabilitySupport::Supported;
  }

  // Projection is live, not a second cache; an external unresolved state does
  // not discard/filter already-owned Store samples or invent capabilities.
  profile.setUnknown();
  EXPECT_TRUE(reader.snapshot(samples.data(), samples.size(), nullptr, 0, metadata).status ==
              VehicleDataReadStatus::Complete);
  EXPECT_TRUE(!metadata.activeIdentity && metadata.capabilityCount == 0);
  EXPECT_TRUE(metadata.sampleCount == 8);
  expectReadSampleEqual(samples[0], expected[0]);
}

inline void testReadModelFailureAtomicCapacity() {
  const auto kamiq = vag::kProfileSet[0].identity;
  ActiveVehicleProfile<1> profile(vag::kAdmittedProfileIdentities);
  EXPECT_TRUE(profile.selectManually(kamiq));
  VehicleDataStore<2> store;
  EXPECT_TRUE(store.upsert(makeNumericSample("one", 1, VehicleSource::Obd,
                                            VehicleQuality::ValidCurrent, 1)) ==
              VehicleDataStoreUpsertResult::Inserted);
  EXPECT_TRUE(store.upsert(makeStatusSample("two", VehicleAvailability::Pending, 2)) ==
              VehicleDataStoreUpsertResult::Inserted);
  const VehicleDataReadModel<1, 2, 4> reader(
      profile, store, kamiq, vag::kKamiqNw4Capabilities);
  std::array<NormalizedVehicleSample, 2> samples{
      makeNumericSample("sentinel", 99, VehicleSource::Derived, VehicleQuality::Stale, 999),
      makeStatusSample("sentinel2", VehicleAvailability::Unknown, 1000)};
  const auto beforeSamples = samples;
  std::array<NormalizedSignalDescriptor, 4> capabilities{};
  capabilities[0] = {NormalizedVehicleSample::SignalId("sentinel"),
                     VehicleValueType::Boolean, NormalizedVehicleSample::Unit("flag"),
                     CapabilitySupport::Unknown};
  const auto beforeCapabilities = capabilities;
  VehicleDataReadSnapshot metadata{ProfileSelectionState::Ambiguous, std::nullopt,
                                  false, 77, 88};
  auto assertFailure = [&](NormalizedVehicleSample* sampleOutput, std::size_t sampleCapacity,
                           NormalizedSignalDescriptor* capabilityOutput,
                           std::size_t capabilityCapacity) {
    const auto result = reader.snapshot(sampleOutput, sampleCapacity, capabilityOutput,
                                        capabilityCapacity, metadata);
    EXPECT_TRUE(result.status == VehicleDataReadStatus::InsufficientCapacity);
    EXPECT_TRUE(result.requiredSampleCapacity == 2 && result.requiredCapabilityCapacity == 4);
    EXPECT_TRUE(metadata.selectionState == ProfileSelectionState::Ambiguous);
    EXPECT_TRUE(!metadata.activeIdentity && !metadata.hasCapabilityRegistry);
    EXPECT_TRUE(metadata.capabilityCount == 77 && metadata.sampleCount == 88);
    for (std::size_t index = 0; index < samples.size(); ++index) {
      expectReadSampleEqual(samples[index], beforeSamples[index]);
    }
    for (std::size_t index = 0; index < capabilities.size(); ++index) {
      expectReadDescriptorEqual(capabilities[index], beforeCapabilities[index]);
    }
  };
  assertFailure(samples.data(), 1, capabilities.data(), 4);
  assertFailure(samples.data(), 2, capabilities.data(), 3);
  assertFailure(samples.data(), 1, capabilities.data(), 3);
  assertFailure(nullptr, 2, capabilities.data(), 4);
  assertFailure(samples.data(), 2, nullptr, 4);
  EXPECT_TRUE(reader.snapshot(samples.data(), 2, capabilities.data(), 4, metadata).status ==
              VehicleDataReadStatus::Complete);
  EXPECT_TRUE(metadata.sampleCount == 2 && metadata.capabilityCount == 4);
}

inline void testReadModelSupportStatesAreNotSampleValues() {
  const ProfileIdentity identity{42};
  ActiveVehicleProfile<1> profile({identity});
  EXPECT_TRUE(profile.selectManually(identity));
  VehicleDataStore<1> store;
  const std::array<NormalizedSignalDescriptor, 4> descriptors{{
      {NormalizedVehicleSample::SignalId("supported"), VehicleValueType::Numeric,
       NormalizedVehicleSample::Unit("unit"), CapabilitySupport::Supported},
      {NormalizedVehicleSample::SignalId("unsupported"), VehicleValueType::Numeric,
       NormalizedVehicleSample::Unit("unit"), CapabilitySupport::Unsupported},
      {NormalizedVehicleSample::SignalId("pending"), VehicleValueType::Numeric,
       NormalizedVehicleSample::Unit("unit"), CapabilitySupport::Pending},
      {NormalizedVehicleSample::SignalId("unknown"), VehicleValueType::Numeric,
       NormalizedVehicleSample::Unit("unit"), CapabilitySupport::Unknown},
  }};
  const FixedSignalRegistry<4> registry(descriptors);
  const VehicleDataReadModel<1, 1, 4> reader(profile, store, identity, registry);
  std::array<NormalizedSignalDescriptor, 4> output{};
  VehicleDataReadSnapshot metadata;
  EXPECT_TRUE(reader.snapshot(nullptr, 0, output.data(), 4, metadata).status ==
              VehicleDataReadStatus::Complete);
  EXPECT_TRUE(metadata.sampleCount == 0 && store.size() == 0);
  for (std::size_t index = 0; index < output.size(); ++index) {
    expectReadDescriptorEqual(output[index], descriptors[index]);
  }
}

inline void runVehicleDataReadModelTests() {
  testReadModelProfileAndCapabilities();
  testReadModelPreservesSamplesAndOwners();
  testReadModelFailureAtomicCapacity();
  testReadModelSupportStatesAreNotSampleValues();
}

}  // namespace vag_data::test
