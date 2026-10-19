#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <limits>

#include "../../src/application/vehicle_data_export_record.h"
#include "test_helpers.h"
#include "utf8_test_data.h"

namespace vag_data::test {

inline bool sameExportSample(const NormalizedVehicleSample& left,
                             const NormalizedVehicleSample& right) {
  if (!(left.signalId() == right.signalId()) ||
      !(left.unit() == right.unit()) || left.valueType() != right.valueType() ||
      left.source() != right.source() || left.quality() != right.quality() ||
      left.availability() != right.availability() ||
      left.timestampMs() != right.timestampMs() ||
      left.hasValue() != right.hasValue()) {
    return false;
  }
  switch (left.valueType()) {
    case VehicleValueType::Numeric: {
      const auto a = left.value().numericValue();
      const auto b = right.value().numericValue();
      return std::memcmp(&a, &b, sizeof(a)) == 0;
    }
    case VehicleValueType::Boolean:
      return left.value().booleanValue() == right.value().booleanValue();
    case VehicleValueType::Text:
      return left.value().textValue() == right.value().textValue();
    case VehicleValueType::None:
      return true;
  }
  return false;
}

inline void expectExportRoundTrip(const NormalizedVehicleSample& input) {
  std::size_t required = 0;
  EXPECT_TRUE(VehicleDataExportRecord::requiredCapacity(input, required) ==
              VehicleDataExportStatus::Ok);
  std::array<std::uint8_t, 256> first{};
  std::array<std::uint8_t, 256> second{};
  std::size_t written = 0;
  EXPECT_TRUE(VehicleDataExportRecord::encode(input, first.data(), first.size(),
                                               written) ==
              VehicleDataExportStatus::Ok);
  EXPECT_TRUE(written == required);
  std::size_t secondWritten = 0;
  EXPECT_TRUE(VehicleDataExportRecord::encode(
                  input, second.data(), second.size(), secondWritten) ==
              VehicleDataExportStatus::Ok);
  EXPECT_TRUE(secondWritten == written);
  EXPECT_TRUE(std::memcmp(first.data(), second.data(), written) == 0);

  NormalizedVehicleSample decoded;
  EXPECT_TRUE(VehicleDataExportRecord::decode(first.data(), written, decoded) ==
              VehicleDataExportStatus::Ok);
  EXPECT_TRUE(sameExportSample(input, decoded));
}

inline void testExportUtf8RoundTripAndMalformedIngress() {
  using Status = VehicleDataExportStatus;
  NormalizedVehicleSample sample;
  EXPECT_TRUE(NormalizedVehicleSample::tryText(
      utf8_cases::kSignalId, utf8_cases::kText, utf8_cases::kUnit,
      VehicleSource::Derived, VehicleQuality::Stale, UINT64_MAX, sample));
  expectExportRoundTrip(sample);
  NormalizedVehicleSample status;
  EXPECT_TRUE(NormalizedVehicleSample::tryStatus(
      utf8_cases::kSignalId, VehicleAvailability::Pending, utf8_cases::kUnit,
      VehicleSource::Unknown, VehicleQuality::InvalidOrUnknown, UINT64_MAX, status));
  expectExportRoundTrip(status);

  std::array<std::uint8_t, 256> bytes{};
  std::size_t written = 0;
  EXPECT_TRUE(VehicleDataExportRecord::encode(sample, bytes.data(), bytes.size(),
                                              written) == Status::Ok);
  const auto signalOffset = VehicleDataExportRecord::kHeaderSize;
  const auto unitOffset = signalOffset + sample.signalId().size();
  const auto textOffset = unitOffset + sample.unit().size();
  EXPECT_TRUE(bytes[4] == 1 && bytes[20] == sample.signalId().size() &&
              bytes[21] == sample.unit().size() &&
              bytes[22] == sample.value().textValue().size());
  EXPECT_TRUE(std::memcmp(bytes.data() + signalOffset, utf8_cases::kSignalId,
                          sample.signalId().size()) == 0);
  EXPECT_TRUE(std::memcmp(bytes.data() + unitOffset, utf8_cases::kUnit,
                          sample.unit().size()) == 0);
  EXPECT_TRUE(std::memcmp(bytes.data() + textOffset, utf8_cases::kText,
                          sample.value().textValue().size()) == 0);
  auto decoded = sample;
  for (const auto offset : {signalOffset, unitOffset, textOffset}) {
    auto malformed = bytes;
    malformed[offset] = 0x80;
    EXPECT_TRUE(VehicleDataExportRecord::decode(malformed.data(), written, decoded) ==
                Status::MalformedRecord);
    EXPECT_TRUE(sameExportSample(decoded, sample));
  }

  // Keep the v1 framing unchanged while replacing only the text payload.
  for (const char* malformed : utf8_cases::kMalformed) {
    auto record = bytes;
    const auto length = std::strlen(malformed);
    const auto recordLength = textOffset + length;
    record[5] = static_cast<std::uint8_t>(recordLength);
    record[6] = static_cast<std::uint8_t>(recordLength >> 8);
    record[22] = static_cast<std::uint8_t>(length);
    std::memcpy(record.data() + textOffset, malformed, length);
    EXPECT_TRUE(VehicleDataExportRecord::decode(record.data(), recordLength, decoded) ==
                Status::MalformedRecord);
    EXPECT_TRUE(sameExportSample(decoded, sample));
  }
}

inline void runLoggerExportTests() {
  testExportUtf8RoundTripAndMalformedIngress();
  using Status = VehicleDataExportStatus;
  using Sample = NormalizedVehicleSample;

  static_assert(VehicleDataExportRecord::kVersion == 1);
  static_assert(VehicleDataExportRecord::kHeaderSize == 23);

  Sample numeric;
  EXPECT_TRUE(Sample::tryNumeric("vehicle.speed", 123.125, "km/h",
                                 VehicleSource::Obd,
                                 VehicleQuality::ValidCurrent,
                                 UINT64_C(18446744073709550000), numeric));
  expectExportRoundTrip(numeric);
  Sample boolean;
  EXPECT_TRUE(Sample::tryBoolean("vehicle.ready", false, "",
                                 VehicleSource::Uds, VehicleQuality::Stale,
                                 UINT64_C(0xFEDCBA9876543210), boolean));
  expectExportRoundTrip(boolean);
  Sample text;
  EXPECT_TRUE(Sample::tryText("vehicle.gear", "D7", "range",
                              VehicleSource::Derived,
                              VehicleQuality::ValidCurrent, 99, text));
  expectExportRoundTrip(text);

  constexpr VehicleAvailability noValueStates[] = {
      VehicleAvailability::Unsupported, VehicleAvailability::Unavailable,
      VehicleAvailability::Pending, VehicleAvailability::Unknown};
  constexpr VehicleSource sources[] = {
      VehicleSource::Obd, VehicleSource::Uds, VehicleSource::PassiveCan,
      VehicleSource::Derived};
  constexpr VehicleQuality qualities[] = {
      VehicleQuality::ValidCurrent, VehicleQuality::Stale,
      VehicleQuality::InvalidOrUnknown, VehicleQuality::ValidCurrent};
  for (std::size_t index = 0; index < 4; ++index) {
    Sample status;
    EXPECT_TRUE(Sample::tryStatus("vehicle.optional", noValueStates[index],
                                  "state", sources[index], qualities[index],
                                  index + 10, status));
    EXPECT_TRUE(!status.hasValue());
    expectExportRoundTrip(status);
  }

  Sample invalidQuality;
  EXPECT_TRUE(Sample::tryNumeric("vehicle.invalid", -0.0, "u",
                                 VehicleSource::Unknown,
                                 VehicleQuality::InvalidOrUnknown, 42,
                                 invalidQuality));
  EXPECT_TRUE(!invalidQuality.hasValue());
  expectExportRoundTrip(invalidQuality);

  Sample maximumText;
  EXPECT_TRUE(Sample::tryText(
      "1234567890123456789012345678901234567890123456789012345678901234",
      "12345678901234567", "12345678901234567890123456789012",
      VehicleSource::PassiveCan, VehicleQuality::Stale, UINT64_MAX,
      maximumText));
  expectExportRoundTrip(maximumText);

  std::size_t required = 0;
  EXPECT_TRUE(VehicleDataExportRecord::requiredCapacity(numeric, required) ==
              Status::Ok);
  std::array<std::uint8_t, 256> exact{};
  std::size_t written = 0;
  EXPECT_TRUE(VehicleDataExportRecord::encode(numeric, exact.data(), required,
                                               written) == Status::Ok);
  EXPECT_TRUE(written == required);

  std::array<std::uint8_t, 256> shortBuffer{};
  shortBuffer.fill(0xA5);
  std::size_t shortRequired = 0;
  EXPECT_TRUE(VehicleDataExportRecord::encode(
                  numeric, shortBuffer.data(), required - 1, shortRequired) ==
              Status::BufferTooSmall);
  EXPECT_TRUE(shortRequired == required);
  for (const auto byte : shortBuffer) {
    EXPECT_TRUE(byte == 0xA5);
  }

  Sample nonFinite;
  EXPECT_TRUE(Sample::tryNumeric("vehicle.bad", std::numeric_limits<double>::infinity(),
                                "u", VehicleSource::Obd,
                                VehicleQuality::ValidCurrent, 1, nonFinite));
  EXPECT_TRUE(VehicleDataExportRecord::requiredCapacity(nonFinite, required) ==
              Status::InvalidSample);

  std::array<std::uint8_t, 256> record{};
  EXPECT_TRUE(VehicleDataExportRecord::encode(numeric, record.data(),
                                               record.size(), written) ==
              Status::Ok);
  Sample unchanged;
  EXPECT_TRUE(Sample::tryBoolean("sentinel", true, "flag", VehicleSource::Uds,
                                 VehicleQuality::Stale, 7, unchanged));
  const auto assertRejected = [&](const std::uint8_t* bytes,
                                  std::size_t length, Status expected) {
    const auto before = unchanged;
    EXPECT_TRUE(VehicleDataExportRecord::decode(bytes, length, unchanged) ==
                expected);
    EXPECT_TRUE(sameExportSample(unchanged, before));
  };
  assertRejected(record.data(), written - 1, Status::MalformedRecord);

  auto malformed = record;
  malformed[0] = 'X';
  assertRejected(malformed.data(), written, Status::MalformedRecord);
  malformed = record;
  malformed[4] = static_cast<std::uint8_t>(VehicleDataExportRecord::kVersion + 1);
  assertRejected(malformed.data(), written, Status::UnsupportedVersion);
  malformed = record;
  malformed[7] = 0xFF;
  assertRejected(malformed.data(), written, Status::MalformedRecord);
  malformed = record;
  malformed[8] = 2;
  assertRejected(malformed.data(), written, Status::MalformedRecord);
  malformed = record;
  malformed[22] = 7;
  assertRejected(malformed.data(), written, Status::MalformedRecord);

  malformed = record;
  const auto numericOffset = VehicleDataExportRecord::kHeaderSize +
                             malformed[20] + malformed[21];
  constexpr std::uint8_t infinityBytes[] = {0x00, 0x00, 0x00, 0x00,
                                             0x00, 0x00, 0xF0, 0x7F};
  std::memcpy(malformed.data() + numericOffset, infinityBytes,
              sizeof(infinityBytes));
  assertRejected(malformed.data(), written, Status::MalformedRecord);

  EXPECT_TRUE(VehicleDataExportRecord::decode(nullptr, written, unchanged) ==
              Status::InvalidArgument);
}

}  // namespace vag_data::test
