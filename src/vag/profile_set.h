#pragma once

#include <array>

#include "../profiles/active_vehicle_profile.h"

namespace vag_data::vag {

enum class ProfileValidationState {
  Pending,
  VehicleConfirmed,
};

struct ProfileDescriptor {
  ProfileIdentity identity;
  const char* modelReference;
  ProfileValidationState validation;
};

// Identity is project-assigned. Kamiq_NW4 is an official model reference,
// not a VIN/chassis matcher or proof of ECU/capability compatibility.
inline constexpr std::array<ProfileDescriptor, 1> kProfileSet{{
    {ProfileIdentity{1}, "Kamiq_NW4", ProfileValidationState::Pending},
}};

inline constexpr std::array<ProfileIdentity, kProfileSet.size()>
    kAdmittedProfileIdentities{{kProfileSet[0].identity}};

}  // namespace vag_data::vag
