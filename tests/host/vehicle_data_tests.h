#pragma once

#include <cstring>
#include <utility>

#include "../../src/core/vehicle_data.h"
#include "test_helpers.h"
#include "utf8_test_data.h"

namespace vag_data::test {

inline void testNormalizedUtf8Ingress() {
  using Sample = NormalizedVehicleSample;
  for (const char* value : utf8_cases::kWellFormed) {
    BoundedText<17> text;
    EXPECT_TRUE(text.assign(value));
    EXPECT_TRUE(text.size() == std::strlen(value));
    EXPECT_TRUE(std::strcmp(text.c_str(), value) == 0);
    NormalizedVehicleValue normalized;
    EXPECT_TRUE(NormalizedVehicleValue::tryText(value, normalized));
    EXPECT_TRUE(normalized.textValue() == text);
  }

  // Capacity is in bytes; this ends exactly at a four-byte scalar boundary.
  constexpr char exact[] = "1234567890123\xF0\x9F\x9A\x97";
  static_assert(sizeof(exact) - 1 == NormalizedVehicleValue::kMaxTextLength);
  BoundedText<17> text(exact);
  EXPECT_TRUE(text.size() == 17);
  EXPECT_TRUE(std::strcmp(text.c_str(), exact) == 0);
  EXPECT_TRUE(!text.assign("12345678901234\xF0\x9F\x9A\x97"));
  EXPECT_TRUE(text.empty());
  EXPECT_TRUE(text.assign("old"));
  EXPECT_TRUE(!text.assign(nullptr));
  EXPECT_TRUE(text.empty() && text.c_str()[0] == '\0');
  BoundedText<0> empty;
  EXPECT_TRUE(empty.assign(""));
  EXPECT_TRUE(!empty.assign("a"));
  const char terminated[] = {'a', '\0', static_cast<char>(0xFF), '\0'};
  EXPECT_TRUE(text.assign(terminated));
  EXPECT_TRUE(text == BoundedText<17>("a"));

  Sample sample;
  EXPECT_TRUE(Sample::tryText(utf8_cases::kSignalId, utf8_cases::kText,
                              utf8_cases::kUnit, VehicleSource::Uds,
                              VehicleQuality::Stale, UINT64_MAX, sample));
  const auto before = sample;
  for (const char* malformed : utf8_cases::kMalformed) {
    EXPECT_TRUE(text.assign("old"));
    EXPECT_TRUE(!text.assign(malformed));
    EXPECT_TRUE(text.empty() && text.c_str()[0] == '\0');
    EXPECT_TRUE(BoundedText<17>(malformed).empty());

    auto value = NormalizedVehicleValue::boolean(true);
    EXPECT_TRUE(!NormalizedVehicleValue::tryText(malformed, value));
    EXPECT_TRUE(value.type() == VehicleValueType::Boolean && value.booleanValue());
    EXPECT_TRUE(!Sample::tryText(utf8_cases::kSignalId, malformed,
                                 utf8_cases::kUnit, VehicleSource::Obd,
                                 VehicleQuality::ValidCurrent, 1, sample));
    EXPECT_TRUE(!Sample::tryText(malformed, "text", "u", VehicleSource::Obd,
                                 VehicleQuality::ValidCurrent, 1, sample));
    EXPECT_TRUE(!Sample::tryText("id", "text", malformed, VehicleSource::Obd,
                                 VehicleQuality::ValidCurrent, 1, sample));
    EXPECT_TRUE(!Sample::tryStatus(malformed, VehicleAvailability::Pending,
                                   "u", VehicleSource::Unknown,
                                   VehicleQuality::InvalidOrUnknown, 1, sample));
    EXPECT_TRUE(!Sample::tryStatus("id", VehicleAvailability::Pending,
                                   malformed, VehicleSource::Unknown,
                                   VehicleQuality::InvalidOrUnknown, 1, sample));
    EXPECT_TRUE(sample.signalId() == before.signalId());
    EXPECT_TRUE(sample.unit() == before.unit());
    EXPECT_TRUE(sample.valueType() == before.valueType());
    EXPECT_TRUE(sample.value().textValue() == before.value().textValue());
    EXPECT_TRUE(sample.source() == before.source());
    EXPECT_TRUE(sample.quality() == before.quality());
    EXPECT_TRUE(sample.availability() == before.availability());
    EXPECT_TRUE(sample.timestampMs() == before.timestampMs());
  }
  auto unchangedValue = NormalizedVehicleValue::boolean(true);
  EXPECT_TRUE(!NormalizedVehicleValue::tryText(nullptr, unchangedValue));
  EXPECT_TRUE(!NormalizedVehicleValue::tryText("123456789012345678", unchangedValue));
  EXPECT_TRUE(unchangedValue.type() == VehicleValueType::Boolean &&
              unchangedValue.booleanValue());
  EXPECT_TRUE(Sample::tryStatus(utf8_cases::kSignalId, VehicleAvailability::Pending,
                                utf8_cases::kUnit, VehicleSource::Unknown,
                                VehicleQuality::InvalidOrUnknown, 2, sample));
  EXPECT_TRUE(sample.signalId() == Sample::SignalId(utf8_cases::kSignalId));
  EXPECT_TRUE(sample.unit() == Sample::Unit(utf8_cases::kUnit));
}

inline void runVehicleDataTests() {
  testNormalizedUtf8Ingress();
  NormalizedVehicleSample numeric;
  EXPECT_TRUE(NormalizedVehicleSample::tryNumeric(
      "engine.speed", 1234.5, "rpm", VehicleSource::Obd,
      VehicleQuality::ValidCurrent, 0x100000001ULL, numeric));
  EXPECT_TRUE(numeric.signalId() == NormalizedVehicleSample::SignalId("engine.speed"));
  EXPECT_TRUE(numeric.valueType() == VehicleValueType::Numeric);
  EXPECT_TRUE(numeric.value().numericValue() == 1234.5);
  EXPECT_TRUE(numeric.unit() == NormalizedVehicleSample::Unit("rpm"));
  EXPECT_TRUE(numeric.source() == VehicleSource::Obd);
  EXPECT_TRUE(numeric.quality() == VehicleQuality::ValidCurrent);
  EXPECT_TRUE(numeric.availability() == VehicleAvailability::Available);
  EXPECT_TRUE(numeric.timestampMs() == 0x100000001ULL);
  EXPECT_TRUE(numeric.isCurrent());

  NormalizedVehicleSample boolean;
  EXPECT_TRUE(NormalizedVehicleSample::tryBoolean(
      "door.open", true, "", VehicleSource::Derived,
      VehicleQuality::ValidCurrent, 42, boolean));
  EXPECT_TRUE(boolean.valueType() == VehicleValueType::Boolean);
  EXPECT_TRUE(boolean.value().booleanValue());
  EXPECT_TRUE(boolean.hasValue());

  char vin[] = "TMB12345678901234";
  NormalizedVehicleSample text;
  EXPECT_TRUE(NormalizedVehicleSample::tryText(
      "vehicle.vin", vin, "", VehicleSource::Uds,
      VehicleQuality::ValidCurrent, 43, text));
  vin[0] = 'X';
  EXPECT_TRUE(std::strcmp(text.value().textValue().c_str(),
                          "TMB12345678901234") == 0);
  EXPECT_TRUE(text.value().textValue().size() == 17);
  EXPECT_TRUE(!NormalizedVehicleSample::tryText(
      "vehicle.vin", "123456789012345678", "", VehicleSource::Uds,
      VehicleQuality::ValidCurrent, 44, text));

  for (const auto availability : {VehicleAvailability::Unsupported,
                                  VehicleAvailability::Unavailable,
                                  VehicleAvailability::Pending,
                                  VehicleAvailability::Unknown}) {
    NormalizedVehicleSample status;
    EXPECT_TRUE(NormalizedVehicleSample::tryStatus(
        "future.signal", availability, "unit", VehicleSource::Unknown,
        VehicleQuality::InvalidOrUnknown, 45, status));
    EXPECT_TRUE(!status.hasValue());
    EXPECT_TRUE(status.valueType() == VehicleValueType::None);
  }

  NormalizedVehicleSample stale;
  EXPECT_TRUE(NormalizedVehicleSample::tryNumeric(
      "engine.speed", 321.0, "rpm", VehicleSource::PassiveCan,
      VehicleQuality::Stale, 46, stale));
  EXPECT_TRUE(stale.hasValue());
  EXPECT_TRUE(!stale.isCurrent());
  EXPECT_TRUE(stale.quality() != numeric.quality());

  NormalizedVehicleSample copied = text;
  EXPECT_TRUE(std::strcmp(copied.value().textValue().c_str(),
                          "TMB12345678901234") == 0);
  NormalizedVehicleSample moved = std::move(copied);
  EXPECT_TRUE(std::strcmp(moved.signalId().c_str(), "vehicle.vin") == 0);
  EXPECT_TRUE(std::strcmp(moved.value().textValue().c_str(),
                          "TMB12345678901234") == 0);
}

}  // namespace vag_data::test
