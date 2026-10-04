#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace vag_data {

// Project-assigned identity only; it carries no route, signal or brand semantics.
class ProfileIdentity {
 public:
  explicit constexpr ProfileIdentity(std::uint32_t value) : value_(value) {}

  constexpr bool operator==(ProfileIdentity other) const {
    return value_ == other.value_;
  }
  constexpr bool operator!=(ProfileIdentity other) const {
    return !(*this == other);
  }

 private:
  std::uint32_t value_;
};

enum class ProfileSelectionState : std::uint8_t {
  Unknown,
  Ambiguous,
  ManualSelectionRequired,
  Selected,
};

// Owns a fixed admitted identity set and manual selection only, not validation.
template <std::size_t ProfileCount>
class ActiveVehicleProfile {
 public:
  explicit ActiveVehicleProfile(
      const std::array<ProfileIdentity, ProfileCount>& admitted)
      : admitted_(admitted) {}

  bool selectManually(ProfileIdentity identity) {
    for (const auto candidate : admitted_) {
      if (candidate == identity) {
        active_ = identity;
        state_ = ProfileSelectionState::Selected;
        return true;
      }
    }
    return false;
  }

  void setUnknown() { clear(ProfileSelectionState::Unknown); }
  void setAmbiguous() { clear(ProfileSelectionState::Ambiguous); }
  void requireManualSelection() {
    clear(ProfileSelectionState::ManualSelectionRequired);
  }

  ProfileSelectionState state() const { return state_; }
  std::optional<ProfileIdentity> activeIdentity() const { return active_; }

 private:
  void clear(ProfileSelectionState state) {
    active_.reset();
    state_ = state;
  }

  std::array<ProfileIdentity, ProfileCount> admitted_;
  ProfileSelectionState state_{ProfileSelectionState::Unknown};
  std::optional<ProfileIdentity> active_;
};

}  // namespace vag_data
