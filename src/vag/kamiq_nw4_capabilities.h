#pragma once

#include "../profiles/signal_registry.h"
#include "profile_set.h"

namespace vag_data::vag {

// Normalized candidate metadata only: no vehicle support or raw mapping claim.
// Literal bounds are checked before the existing BoundedText constructors run.
static_assert(sizeof("vehicle.coolantTemp") - 1 <=
              NormalizedVehicleSample::kMaxSignalIdLength);
static_assert(sizeof("km/h") - 1 <= NormalizedVehicleSample::kMaxUnitLength);
inline const FixedSignalRegistry<4> kKamiqNw4Capabilities{
    std::array<NormalizedSignalDescriptor, 4>{{
        {NormalizedVehicleSample::SignalId("vehicle.speed"),
         VehicleValueType::Numeric, NormalizedVehicleSample::Unit("km/h"),
         CapabilitySupport::Pending},
        {NormalizedVehicleSample::SignalId("vehicle.rpm"),
         VehicleValueType::Numeric, NormalizedVehicleSample::Unit("rpm"),
         CapabilitySupport::Pending},
        {NormalizedVehicleSample::SignalId("vehicle.coolantTemp"),
         VehicleValueType::Numeric, NormalizedVehicleSample::Unit("degC"),
         CapabilitySupport::Pending},
        {NormalizedVehicleSample::SignalId("vehicle.voltage"),
         VehicleValueType::Numeric, NormalizedVehicleSample::Unit("V"),
         CapabilitySupport::Pending},
    }}};

// Explicit profile association only; this is not detection or support learning.
inline const FixedSignalRegistry<4>* signalRegistryFor(ProfileIdentity identity) {
  return identity == kProfileSet[0].identity ? &kKamiqNw4Capabilities : nullptr;
}

}  // namespace vag_data::vag
