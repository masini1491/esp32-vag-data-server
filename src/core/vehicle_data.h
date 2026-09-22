#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace vag_data {

template <std::size_t Capacity>
class BoundedText {
 public:
  BoundedText() = default;

  explicit BoundedText(const char* value) { assign(value); }

  bool assign(const char* value) {
    if (value == nullptr) {
      clear();
      return false;
    }
    const auto length = std::strlen(value);
    if (length > Capacity) {
      clear();
      return false;
    }
    std::memcpy(data_.data(), value, length);
    data_[length] = '\0';
    length_ = length;
    return true;
  }

  void clear() {
    data_[0] = '\0';
    length_ = 0;
  }

  const char* c_str() const { return data_.data(); }
  std::size_t size() const { return length_; }
  bool empty() const { return length_ == 0; }

  bool operator==(const BoundedText& other) const {
    return length_ == other.length_ &&
           std::memcmp(data_.data(), other.data_.data(), length_) == 0;
  }

 private:
  std::array<char, Capacity + 1> data_{};
  std::size_t length_{0};
};

enum class VehicleValueType : std::uint8_t {
  None,
  Numeric,
  Boolean,
  Text,
};

enum class VehicleAvailability : std::uint8_t {
  Available,
  Unsupported,
  Unavailable,
  Pending,
  Unknown,
};

enum class VehicleQuality : std::uint8_t {
  ValidCurrent,
  Stale,
  InvalidOrUnknown,
};

enum class VehicleSource : std::uint8_t {
  Obd,
  Uds,
  PassiveCan,
  Derived,
  Unknown,
};

class NormalizedVehicleValue {
 public:
  static constexpr std::size_t kMaxTextLength = 17;
  using Text = BoundedText<kMaxTextLength>;

  static NormalizedVehicleValue numeric(double value) {
    NormalizedVehicleValue result;
    result.type_ = VehicleValueType::Numeric;
    result.numeric_ = value;
    return result;
  }

  static NormalizedVehicleValue boolean(bool value) {
    NormalizedVehicleValue result;
    result.type_ = VehicleValueType::Boolean;
    result.boolean_ = value;
    return result;
  }

  static bool tryText(const char* value, NormalizedVehicleValue& result) {
    Text text;
    if (!text.assign(value)) {
      return false;
    }
    result = NormalizedVehicleValue{};
    result.type_ = VehicleValueType::Text;
    result.text_ = text;
    return true;
  }

  VehicleValueType type() const { return type_; }
  double numericValue() const { return numeric_; }
  bool booleanValue() const { return boolean_; }
  const Text& textValue() const { return text_; }

 private:
  VehicleValueType type_{VehicleValueType::None};
  double numeric_{0.0};
  bool boolean_{false};
  Text text_{};
};

class NormalizedVehicleSample {
 public:
  static constexpr std::size_t kMaxSignalIdLength = 64;
  static constexpr std::size_t kMaxUnitLength = 32;
  using SignalId = BoundedText<kMaxSignalIdLength>;
  using Unit = BoundedText<kMaxUnitLength>;

  static bool tryNumeric(const char* signalId, double value, const char* unit,
                         VehicleSource source, VehicleQuality quality,
                         std::uint64_t timestampMs,
                         NormalizedVehicleSample& result) {
    return makeAvailable(signalId, unit, source, quality, timestampMs,
                         NormalizedVehicleValue::numeric(value), result);
  }

  static bool tryBoolean(const char* signalId, bool value, const char* unit,
                         VehicleSource source, VehicleQuality quality,
                         std::uint64_t timestampMs,
                         NormalizedVehicleSample& result) {
    return makeAvailable(signalId, unit, source, quality, timestampMs,
                         NormalizedVehicleValue::boolean(value), result);
  }

  static bool tryText(const char* signalId, const char* value, const char* unit,
                      VehicleSource source, VehicleQuality quality,
                      std::uint64_t timestampMs,
                      NormalizedVehicleSample& result) {
    NormalizedVehicleValue normalizedValue;
    if (!NormalizedVehicleValue::tryText(value, normalizedValue)) {
      return false;
    }
    return makeAvailable(signalId, unit, source, quality, timestampMs,
                         normalizedValue, result);
  }

  static bool tryStatus(const char* signalId, VehicleAvailability availability,
                        const char* unit, VehicleSource source,
                        VehicleQuality quality, std::uint64_t timestampMs,
                        NormalizedVehicleSample& result) {
    if (availability == VehicleAvailability::Available) {
      return false;
    }
    SignalId normalizedId;
    Unit normalizedUnit;
    if (!normalizedId.assign(signalId) || !normalizedUnit.assign(unit)) {
      return false;
    }
    result = NormalizedVehicleSample{};
    result.signalId_ = normalizedId;
    result.unit_ = normalizedUnit;
    result.source_ = source;
    result.quality_ = quality;
    result.availability_ = availability;
    result.timestampMs_ = timestampMs;
    return true;
  }

  const SignalId& signalId() const { return signalId_; }
  const Unit& unit() const { return unit_; }
  VehicleValueType valueType() const { return value_.type(); }
  const NormalizedVehicleValue& value() const { return value_; }
  VehicleSource source() const { return source_; }
  VehicleQuality quality() const { return quality_; }
  VehicleAvailability availability() const { return availability_; }
  std::uint64_t timestampMs() const { return timestampMs_; }

  bool hasValue() const {
    return availability_ == VehicleAvailability::Available &&
           value_.type() != VehicleValueType::None &&
           quality_ != VehicleQuality::InvalidOrUnknown;
  }

  bool isCurrent() const {
    return hasValue() && quality_ == VehicleQuality::ValidCurrent;
  }

 private:
  static bool makeAvailable(const char* signalId, const char* unit,
                            VehicleSource source, VehicleQuality quality,
                            std::uint64_t timestampMs,
                            const NormalizedVehicleValue& value,
                            NormalizedVehicleSample& result) {
    SignalId normalizedId;
    Unit normalizedUnit;
    if (!normalizedId.assign(signalId) || !normalizedUnit.assign(unit) ||
        value.type() == VehicleValueType::None) {
      return false;
    }
    result = NormalizedVehicleSample{};
    result.signalId_ = normalizedId;
    result.unit_ = normalizedUnit;
    result.value_ = value;
    result.source_ = source;
    result.quality_ = quality;
    result.availability_ = VehicleAvailability::Available;
    result.timestampMs_ = timestampMs;
    return true;
  }

  SignalId signalId_{};
  Unit unit_{};
  NormalizedVehicleValue value_{};
  VehicleSource source_{VehicleSource::Unknown};
  VehicleQuality quality_{VehicleQuality::InvalidOrUnknown};
  VehicleAvailability availability_{VehicleAvailability::Unknown};
  std::uint64_t timestampMs_{0};
};

}  // namespace vag_data
