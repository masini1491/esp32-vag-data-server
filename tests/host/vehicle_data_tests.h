#pragma once

#include <cstring>
#include <utility>

#include "../../src/core/vehicle_data.h"
#include "test_helpers.h"

namespace vag_data::test {

inline void runVehicleDataTests() {
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
