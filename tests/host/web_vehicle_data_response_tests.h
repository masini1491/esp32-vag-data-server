#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

#include "../../src/application/web_vehicle_data_response.h"
#include "../../src/vag/profile_set.h"
#include "test_helpers.h"

namespace vag_data::test {

inline void runWebVehicleDataResponseTests() {
  std::array<NormalizedSignalDescriptor, 4> descriptors{{
      {NormalizedVehicleSample::SignalId("vehicle.speed"),
       VehicleValueType::Numeric, NormalizedVehicleSample::Unit("km/h"),
       CapabilitySupport::Pending},
      {NormalizedVehicleSample::SignalId("vehicle.rpm"),
       VehicleValueType::Numeric, NormalizedVehicleSample::Unit("rpm"),
       CapabilitySupport::Pending},
      {NormalizedVehicleSample::SignalId("vehicle.coolantTemp"),
       VehicleValueType::Numeric, NormalizedVehicleSample::Unit("Cel"),
       CapabilitySupport::Pending},
      {NormalizedVehicleSample::SignalId("vehicle.voltage"),
       VehicleValueType::Numeric, NormalizedVehicleSample::Unit("V"),
       CapabilitySupport::Pending},
  }};
  const FixedSignalRegistry<4> registry(descriptors);
  ActiveVehicleProfile<1> profile(vag::kAdmittedProfileIdentities);
  VehicleDataStore<7> store;
  VehicleDataReadModel<1, 7, 4> readModel(
      profile, store, vag::kProfileSet[0].identity, registry);

  VehicleDataReadSnapshot metadata;
  std::array<NormalizedVehicleSample, 7> samples{};
  std::array<NormalizedSignalDescriptor, 4> copiedCapabilities{};
  EXPECT_TRUE(readModel.snapshot(samples.data(), samples.size(),
                                copiedCapabilities.data(),
                                copiedCapabilities.size(), metadata).status ==
              VehicleDataReadStatus::Complete);
  std::array<char, 2048> output{};
  auto result = serializeWebVehicleDataResponse(
      metadata, copiedCapabilities.data(), samples.data(), output.data(),
      output.size());
  EXPECT_TRUE(result.status == WebResponseSerializationStatus::Serialized);
  const std::string unresolved(output.data());
  EXPECT_TRUE(unresolved.find("\"profileSelection\":\"unknown\"") !=
              std::string::npos);
  EXPECT_TRUE(unresolved.find("\"activeProfileId\"") == std::string::npos);
  EXPECT_TRUE(unresolved.find("\"capabilities\":null") != std::string::npos);
  EXPECT_TRUE(unresolved.find("\"samples\":[]") != std::string::npos);

  EXPECT_TRUE(profile.selectManually(vag::kProfileSet[0].identity));
  EXPECT_TRUE(readModel.snapshot(samples.data(), samples.size(),
                                copiedCapabilities.data(),
                                copiedCapabilities.size(), metadata).status ==
              VehicleDataReadStatus::Complete);
  output.fill('\0');
  result = serializeWebVehicleDataResponse(
      metadata, copiedCapabilities.data(), samples.data(), output.data(),
      output.size());
  EXPECT_TRUE(result.status == WebResponseSerializationStatus::Serialized);
  EXPECT_TRUE(std::string(output.data()).find("\"activeProfileId\":1") !=
              std::string::npos);
  EXPECT_TRUE(std::string(output.data()).find("\"support\":\"pending\"") !=
              std::string::npos);
  EXPECT_TRUE(std::string(output.data()).find("\"samples\":[]") !=
              std::string::npos);

  NormalizedVehicleSample speed;
  EXPECT_TRUE(NormalizedVehicleSample::tryNumeric(
      "vehicle.speed", 42.5, "km/h", VehicleSource::Obd,
      VehicleQuality::Stale, 9007199254740993ULL, speed));
  EXPECT_TRUE(store.upsert(speed) == VehicleDataStoreUpsertResult::Inserted);
  NormalizedVehicleSample unavailable;
  EXPECT_TRUE(NormalizedVehicleSample::tryStatus(
      "vehicle.coolantTemp", VehicleAvailability::Unavailable, "Cel",
      VehicleSource::Unknown, VehicleQuality::InvalidOrUnknown, 55,
      unavailable));
  EXPECT_TRUE(store.upsert(unavailable) == VehicleDataStoreUpsertResult::Inserted);
  NormalizedVehicleSample pending;
  EXPECT_TRUE(NormalizedVehicleSample::tryStatus(
      "vehicle.voltage", VehicleAvailability::Pending, "V",
      VehicleSource::Unknown, VehicleQuality::InvalidOrUnknown, 56, pending));
  EXPECT_TRUE(store.upsert(pending) == VehicleDataStoreUpsertResult::Inserted);
  NormalizedVehicleSample unknown;
  EXPECT_TRUE(NormalizedVehicleSample::tryStatus(
      "vehicle.gear", VehicleAvailability::Unknown, "",
      VehicleSource::Unknown, VehicleQuality::InvalidOrUnknown, 57, unknown));
  EXPECT_TRUE(store.upsert(unknown) == VehicleDataStoreUpsertResult::Inserted);
  NormalizedVehicleSample booleanSample;
  EXPECT_TRUE(NormalizedVehicleSample::tryBoolean(
      "vehicle.exampleFlag", false, "", VehicleSource::Derived,
      VehicleQuality::ValidCurrent, 58, booleanSample));
  EXPECT_TRUE(store.upsert(booleanSample) == VehicleDataStoreUpsertResult::Inserted);
  NormalizedVehicleSample textSample;
  EXPECT_TRUE(NormalizedVehicleSample::tryText(
      "vehicle.exampleText", "ready\"ok", "", VehicleSource::Derived,
      VehicleQuality::ValidCurrent, 59, textSample));
  EXPECT_TRUE(store.upsert(textSample) == VehicleDataStoreUpsertResult::Inserted);
  NormalizedVehicleSample unsupported;
  EXPECT_TRUE(NormalizedVehicleSample::tryStatus(
      "vehicle.exampleUnsupported", VehicleAvailability::Unsupported, "",
      VehicleSource::Unknown, VehicleQuality::InvalidOrUnknown, 60,
      unsupported));
  EXPECT_TRUE(store.upsert(unsupported) == VehicleDataStoreUpsertResult::Inserted);

  EXPECT_TRUE(readModel.snapshot(samples.data(), samples.size(),
                                copiedCapabilities.data(),
                                copiedCapabilities.size(), metadata).status ==
              VehicleDataReadStatus::Complete);
  output.fill('\0');
  result = serializeWebVehicleDataResponse(
      metadata, copiedCapabilities.data(), samples.data(), output.data(),
      output.size());
  EXPECT_TRUE(result.status == WebResponseSerializationStatus::Serialized);
  const std::string selected(output.data());
  EXPECT_TRUE(selected.find("\"activeProfileId\":1") != std::string::npos);
  EXPECT_TRUE(selected.find("\"capabilities\":[") != std::string::npos);
  EXPECT_TRUE(selected.find("\"support\":\"pending\"") != std::string::npos);
  EXPECT_TRUE(selected.find("\"value\":42.5") != std::string::npos);
  EXPECT_TRUE(selected.find("\"source\":\"obd\"") != std::string::npos);
  EXPECT_TRUE(selected.find("\"quality\":\"stale\"") != std::string::npos);
  EXPECT_TRUE(selected.find("\"availability\":\"unavailable\"") !=
              std::string::npos);
  EXPECT_TRUE(selected.find("\"availability\":\"unsupported\"") !=
              std::string::npos);
  EXPECT_TRUE(selected.find("\"availability\":\"pending\"") !=
              std::string::npos);
  EXPECT_TRUE(selected.find("\"availability\":\"unknown\"") !=
              std::string::npos);
  EXPECT_TRUE(selected.find("\"timestampMs\":\"9007199254740993\"") !=
              std::string::npos);
  EXPECT_TRUE(selected.find("\"signalId\":\"vehicle.gear\",\"unit\":\"\"") !=
              std::string::npos);
  EXPECT_TRUE(selected.find("\"signalId\":\"vehicle.coolantTemp\",\"unit\":\"Cel\",\"source\"") !=
              std::string::npos);
  EXPECT_TRUE(selected.find("\"signalId\":\"vehicle.coolantTemp\",\"unit\":\"Cel\",\"value\"") ==
              std::string::npos);
  EXPECT_TRUE(selected.find("\"valueType\":\"boolean\",\"value\":false") !=
              std::string::npos);
  EXPECT_TRUE(selected.find("ready\\\"ok") != std::string::npos);

  const NormalizedSignalDescriptor mixedCapabilities[] = {
      {NormalizedVehicleSample::SignalId("signal.supported"),
       VehicleValueType::Numeric, NormalizedVehicleSample::Unit("u"),
       CapabilitySupport::Supported},
      {NormalizedVehicleSample::SignalId("signal.unsupported"),
       VehicleValueType::Numeric, NormalizedVehicleSample::Unit("u"),
       CapabilitySupport::Unsupported},
      {NormalizedVehicleSample::SignalId("signal.pending"),
       VehicleValueType::Numeric, NormalizedVehicleSample::Unit("u"),
       CapabilitySupport::Pending},
      {NormalizedVehicleSample::SignalId("signal.unknown"),
       VehicleValueType::Numeric, NormalizedVehicleSample::Unit("u"),
       CapabilitySupport::Unknown},
  };
  VehicleDataReadSnapshot mixedMetadata;
  mixedMetadata.selectionState = ProfileSelectionState::Unknown;
  mixedMetadata.hasCapabilityRegistry = true;
  mixedMetadata.capabilityCount = 4;
  std::array<char, 1200> mixedOutput{};
  const auto mixedResult = serializeWebVehicleDataResponse(
      mixedMetadata, mixedCapabilities, nullptr, mixedOutput.data(),
      mixedOutput.size());
  EXPECT_TRUE(mixedResult.status == WebResponseSerializationStatus::Serialized);
  const std::string mixed(mixedOutput.data());
  EXPECT_TRUE(mixed.find("\"support\":\"supported\"") != std::string::npos);
  EXPECT_TRUE(mixed.find("\"support\":\"unsupported\"") != std::string::npos);
  EXPECT_TRUE(mixed.find("\"support\":\"pending\"") != std::string::npos);
  EXPECT_TRUE(mixed.find("\"support\":\"unknown\"") != std::string::npos);

  std::array<char, 2048> repeated{};
  const auto repeatedResult = serializeWebVehicleDataResponse(
      metadata, copiedCapabilities.data(), samples.data(), repeated.data(),
      repeated.size());
  EXPECT_TRUE(repeatedResult.status == WebResponseSerializationStatus::Serialized);
  EXPECT_TRUE(std::strcmp(output.data(), repeated.data()) == 0);
  EXPECT_TRUE(result.requiredCapacity == repeatedResult.requiredCapacity);

  std::array<char, 8> shortOutput;
  shortOutput.fill('x');
  const auto shortResult = serializeWebVehicleDataResponse(
      metadata, copiedCapabilities.data(), samples.data(), shortOutput.data(),
      shortOutput.size());
  EXPECT_TRUE(shortResult.status ==
              WebResponseSerializationStatus::InsufficientCapacity);
  EXPECT_TRUE(shortResult.requiredCapacity > shortOutput.size());
  for (const char value : shortOutput) EXPECT_TRUE(value == 'x');

  NormalizedVehicleSample nonFinite;
  EXPECT_TRUE(NormalizedVehicleSample::tryNumeric(
      "vehicle.speed", std::numeric_limits<double>::quiet_NaN(), "km/h",
      VehicleSource::Obd, VehicleQuality::ValidCurrent, 99, nonFinite));
  const NormalizedVehicleSample invalidSamples[] = {nonFinite};
  std::array<char, 64> invalidOutput;
  invalidOutput.fill('z');
  VehicleDataReadSnapshot invalidMetadata;
  invalidMetadata.selectionState = ProfileSelectionState::Unknown;
  invalidMetadata.hasCapabilityRegistry = false;
  invalidMetadata.sampleCount = 1;
  const auto invalidResult = serializeWebVehicleDataResponse(
      invalidMetadata, nullptr, invalidSamples, invalidOutput.data(),
      invalidOutput.size());
  EXPECT_TRUE(invalidResult.status == WebResponseSerializationStatus::InvalidInput);
  for (const char value : invalidOutput) EXPECT_TRUE(value == 'z');
}

}  // namespace vag_data::test
