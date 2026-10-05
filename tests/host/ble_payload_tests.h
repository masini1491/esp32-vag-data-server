#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

#include "../../src/application/ble_vehicle_data_payload.h"
#include "../../src/vag/kamiq_nw4_capabilities.h"
#include "ble_payload_decoder.h"
#include "test_helpers.h"

namespace vag_data::test {

template <typename Encode>
inline std::size_t encodePayload(Encode encode,
                                 std::array<std::uint8_t, 256>& output) {
  const auto sizing = encode(nullptr, 0);
  EXPECT_TRUE(sizing.status == BlePayloadStatus::InsufficientCapacity);
  EXPECT_TRUE(sizing.requiredCapacity > 0 &&
              sizing.requiredCapacity <= output.size());
  const auto result = encode(output.data(), sizing.requiredCapacity);
  EXPECT_TRUE(result.status == BlePayloadStatus::Encoded);
  EXPECT_TRUE(result.requiredCapacity == sizing.requiredCapacity);
  return result.requiredCapacity;
}

inline void testBleSnapshotMetadataRecords() {
  std::array<std::uint8_t, 256> bytes{};
  const VehicleDataReadSnapshot unresolved{};
  const auto unresolvedSize = encodePayload(
      [&](std::uint8_t* out, std::size_t cap) {
        return encodeBleSnapshotMetadata(unresolved, out, cap);
      },
      bytes);
  DecodedBlePayload decoded;
  EXPECT_TRUE(decodeBlePayload(bytes.data(), unresolvedSize, decoded));
  EXPECT_TRUE(decoded.recordType == BlePayloadRecordType::SnapshotMetadata);
  EXPECT_TRUE(decoded.selectionState == ProfileSelectionState::Unknown);
  EXPECT_TRUE(!decoded.hasActiveIdentity && !decoded.hasCapabilityRegistry);
  EXPECT_TRUE(decoded.capabilityCount == 0 && decoded.sampleCount == 0);

  const VehicleDataReadSnapshot selected{ProfileSelectionState::Selected,
                                         ProfileIdentity{1}, true, 4, 0};
  const auto selectedSize = encodePayload(
      [&](std::uint8_t* out, std::size_t cap) {
        return encodeBleSnapshotMetadata(selected, out, cap);
      },
      bytes);
  EXPECT_TRUE(decodeBlePayload(bytes.data(), selectedSize, decoded));
  EXPECT_TRUE(decoded.selectionState == ProfileSelectionState::Selected);
  EXPECT_TRUE(decoded.hasActiveIdentity && decoded.activeIdentity == 1);
  EXPECT_TRUE(decoded.hasCapabilityRegistry && decoded.capabilityCount == 4);
  EXPECT_TRUE(decoded.sampleCount == 0);
}

inline void testBleCapabilityRecords() {
  const auto& descriptors = vag::kKamiqNw4Capabilities.descriptors();
  std::array<std::uint8_t, 256> bytes{};
  for (std::size_t index = 0; index < descriptors.size(); ++index) {
    const auto size = encodePayload(
        [&](std::uint8_t* out, std::size_t cap) {
          return encodeBleCapabilityRecord(descriptors[index], index, out, cap);
        },
        bytes);
    DecodedBlePayload decoded;
    EXPECT_TRUE(decodeBlePayload(bytes.data(), size, decoded));
    EXPECT_TRUE(decoded.recordType == BlePayloadRecordType::Capability);
    EXPECT_TRUE(decoded.index == index);
    EXPECT_TRUE(decoded.capability.identity == descriptors[index].identity);
    EXPECT_TRUE(decoded.capability.expectedValueType ==
                descriptors[index].expectedValueType);
    EXPECT_TRUE(decoded.capability.unit == descriptors[index].unit);
    EXPECT_TRUE(decoded.capability.support == CapabilitySupport::Pending);
  }

  const std::array<NormalizedSignalDescriptor, 4> supportStates{{
      {NormalizedVehicleSample::SignalId("supported"), VehicleValueType::Numeric,
       NormalizedVehicleSample::Unit("unit"), CapabilitySupport::Supported},
      {NormalizedVehicleSample::SignalId("unsupported"), VehicleValueType::Boolean,
       NormalizedVehicleSample::Unit("bool"), CapabilitySupport::Unsupported},
      {NormalizedVehicleSample::SignalId("pending"), VehicleValueType::Text,
       NormalizedVehicleSample::Unit("text"), CapabilitySupport::Pending},
      {NormalizedVehicleSample::SignalId("unknown"), VehicleValueType::Numeric,
       NormalizedVehicleSample::Unit("unit"), CapabilitySupport::Unknown},
  }};
  for (std::size_t index = 0; index < supportStates.size(); ++index) {
    const auto size = encodePayload(
        [&](std::uint8_t* out, std::size_t cap) {
          return encodeBleCapabilityRecord(supportStates[index], index, out, cap);
        },
        bytes);
    DecodedBlePayload decoded;
    EXPECT_TRUE(decodeBlePayload(bytes.data(), size, decoded));
    EXPECT_TRUE(decoded.capability.support == supportStates[index].support);
    EXPECT_TRUE(decoded.capability.expectedValueType ==
                supportStates[index].expectedValueType);
  }
}

inline void testBleSampleRecordsAndTimestampRoundTrip() {
  using Sample = NormalizedVehicleSample;
  constexpr std::uint64_t exactTimestamp = 0xFEDCBA9876543210ULL;
  std::array<Sample, 7> samples{};
  EXPECT_TRUE(Sample::tryNumeric("vehicle.speed", 123.5, "km/h",
                                 VehicleSource::Obd,
                                 VehicleQuality::ValidCurrent, exactTimestamp,
                                 samples[0]));
  EXPECT_TRUE(Sample::tryBoolean("flag", true, "bool", VehicleSource::Derived,
                                 VehicleQuality::Stale, 42, samples[1]));
  EXPECT_TRUE(Sample::tryText("identity", "A\\B", "text", VehicleSource::Uds,
                              VehicleQuality::ValidCurrent, 43, samples[2]));
  std::size_t index = 3;
  for (const auto availability : {VehicleAvailability::Unsupported,
                                  VehicleAvailability::Unavailable,
                                  VehicleAvailability::Pending,
                                  VehicleAvailability::Unknown}) {
    const char* ids[] = {"unsupported", "unavailable", "pending", "unknown"};
    EXPECT_TRUE(Sample::tryStatus(ids[index - 3], availability, "unit",
                                  VehicleSource::Unknown,
                                  VehicleQuality::InvalidOrUnknown, index,
                                  samples[index]));
    ++index;
  }

  std::array<std::uint8_t, 256> bytes{};
  for (index = 0; index < samples.size(); ++index) {
    const auto size = encodePayload(
        [&](std::uint8_t* out, std::size_t cap) {
          return encodeBleSampleRecord(samples[index], index, out, cap);
        },
        bytes);
    const auto saved = bytes;
    const auto repeated = encodeBleSampleRecord(samples[index], index,
                                                 bytes.data(), size);
    EXPECT_TRUE(repeated.status == BlePayloadStatus::Encoded);
    EXPECT_TRUE(bytes == saved);

    DecodedBlePayload decoded;
    EXPECT_TRUE(decodeBlePayload(bytes.data(), size, decoded));
    EXPECT_TRUE(decoded.recordType == BlePayloadRecordType::Sample);
    EXPECT_TRUE(decoded.index == index);
    EXPECT_TRUE(decoded.signalId == samples[index].signalId());
    EXPECT_TRUE(decoded.unit == samples[index].unit());
    EXPECT_TRUE(decoded.valueType == samples[index].valueType());
    EXPECT_TRUE(decoded.hasValue == samples[index].hasValue());
    EXPECT_TRUE(decoded.source == samples[index].source());
    EXPECT_TRUE(decoded.quality == samples[index].quality());
    EXPECT_TRUE(decoded.availability == samples[index].availability());
    EXPECT_TRUE(decoded.timestampMs == samples[index].timestampMs());
    if (index == 0) {
      EXPECT_TRUE(decoded.numericValue == 123.5);
      EXPECT_TRUE(decoded.timestampMs == exactTimestamp);
    } else if (index == 1) {
      EXPECT_TRUE(decoded.booleanValue);
    } else if (index == 2) {
      EXPECT_TRUE(decoded.textValue == NormalizedVehicleValue::Text("A\\B"));
    } else {
      EXPECT_TRUE(!decoded.hasValue);
    }
  }
}

inline void testBlePayloadCapacityDeterminismAndMalformedInput() {
  std::array<std::uint8_t, 256> bytes{};
  NormalizedVehicleSample numeric;
  EXPECT_TRUE(NormalizedVehicleSample::tryNumeric(
      "vehicle.speed", 1.25, "km/h", VehicleSource::Obd,
      VehicleQuality::ValidCurrent, 99, numeric));
  const auto sizing = encodeBleSampleRecord(numeric, 0, nullptr, 0);
  EXPECT_TRUE(sizing.status == BlePayloadStatus::InsufficientCapacity);
  EXPECT_TRUE(sizing.requiredCapacity > 1);

  bytes.fill(0xA5);
  const auto before = bytes;
  auto shortResult = encodeBleSampleRecord(numeric, 0, bytes.data(),
                                           sizing.requiredCapacity - 1);
  EXPECT_TRUE(shortResult.status == BlePayloadStatus::InsufficientCapacity);
  EXPECT_TRUE(shortResult.requiredCapacity == sizing.requiredCapacity);
  EXPECT_TRUE(bytes == before);
  const auto exact = encodeBleSampleRecord(numeric, 0, bytes.data(),
                                            sizing.requiredCapacity);
  EXPECT_TRUE(exact.status == BlePayloadStatus::Encoded);
  const auto canonical = bytes;
  EXPECT_TRUE(encodeBleSampleRecord(numeric, 0, bytes.data(),
                                    sizing.requiredCapacity).status ==
              BlePayloadStatus::Encoded);
  EXPECT_TRUE(bytes == canonical);

  auto inconsistent = VehicleDataReadSnapshot{
      ProfileSelectionState::Selected, std::nullopt, false, 0, 0};
  EXPECT_TRUE(encodeBleSnapshotMetadata(inconsistent, bytes.data(), bytes.size()).status ==
              BlePayloadStatus::InvalidInput);

  const NormalizedSignalDescriptor invalidCapability{
      NormalizedVehicleSample::SignalId("invalid"), VehicleValueType::Numeric,
      NormalizedVehicleSample::Unit("unit"),
      static_cast<CapabilitySupport>(0xFF)};
  EXPECT_TRUE(encodeBleCapabilityRecord(invalidCapability, 0, bytes.data(),
                                        bytes.size()).status ==
              BlePayloadStatus::InvalidInput);
  EXPECT_TRUE(encodeBleCapabilityRecord(vag::kKamiqNw4Capabilities.descriptors()[0],
                                        65536, bytes.data(), bytes.size()).status ==
              BlePayloadStatus::InvalidInput);

  NormalizedVehicleSample nonFinite;
  EXPECT_TRUE(NormalizedVehicleSample::tryNumeric(
      "invalid", std::numeric_limits<double>::infinity(), "unit",
      VehicleSource::Unknown, VehicleQuality::ValidCurrent, 1, nonFinite));
  EXPECT_TRUE(encodeBleSampleRecord(nonFinite, 0, bytes.data(), bytes.size()).status ==
              BlePayloadStatus::InvalidInput);

  DecodedBlePayload decoded;
  EXPECT_TRUE(!decodeBlePayload(nullptr, 1, decoded));
  const std::array<std::uint8_t, 2> badVersion{{2, 1}};
  EXPECT_TRUE(!decodeBlePayload(badVersion.data(), badVersion.size(), decoded));
  const std::array<std::uint8_t, 2> truncated{{1, 1}};
  EXPECT_TRUE(!decodeBlePayload(truncated.data(), truncated.size(), decoded));
}

inline void runBlePayloadTests() {
  testBleSnapshotMetadataRecords();
  testBleCapabilityRecords();
  testBleSampleRecordsAndTimestampRoundTrip();
  testBlePayloadCapacityDeterminismAndMalformedInput();
}

}  // namespace vag_data::test
