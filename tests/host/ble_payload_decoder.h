#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "../../src/application/ble_vehicle_data_payload.h"

namespace vag_data::test {

struct DecodedBlePayload {
  BlePayloadRecordType recordType{BlePayloadRecordType::SnapshotMetadata};
  std::size_t index{0};

  ProfileSelectionState selectionState{ProfileSelectionState::Unknown};
  bool hasActiveIdentity{false};
  std::uint32_t activeIdentity{0};
  bool hasCapabilityRegistry{false};
  std::uint16_t capabilityCount{0};
  std::uint16_t sampleCount{0};

  NormalizedSignalDescriptor capability{};

  NormalizedVehicleSample::SignalId signalId{};
  NormalizedVehicleSample::Unit unit{};
  VehicleValueType valueType{VehicleValueType::None};
  bool hasValue{false};
  double numericValue{0};
  bool booleanValue{false};
  NormalizedVehicleValue::Text textValue{};
  VehicleSource source{VehicleSource::Unknown};
  VehicleQuality quality{VehicleQuality::InvalidOrUnknown};
  VehicleAvailability availability{VehicleAvailability::Unknown};
  std::uint64_t timestampMs{0};
};

class BlePayloadReader {
 public:
  BlePayloadReader(const std::uint8_t* bytes, std::size_t length)
      : bytes_(bytes), length_(length) {}

  bool byte(std::uint8_t& value) {
    if (offset_ >= length_) return false;
    value = bytes_[offset_++];
    return true;
  }

  bool u16(std::uint16_t& value) {
    std::uint8_t high = 0, low = 0;
    if (!byte(high) || !byte(low)) return false;
    value = static_cast<std::uint16_t>((static_cast<std::uint16_t>(high) << 8) |
                                       low);
    return true;
  }

  bool u32(std::uint32_t& value) {
    value = 0;
    for (int i = 0; i < 4; ++i) {
      std::uint8_t part = 0;
      if (!byte(part)) return false;
      value = (value << 8) | part;
    }
    return true;
  }

  bool u64(std::uint64_t& value) {
    value = 0;
    for (int i = 0; i < 8; ++i) {
      std::uint8_t part = 0;
      if (!byte(part)) return false;
      value = (value << 8) | part;
    }
    return true;
  }

  template <std::size_t Capacity>
  bool string(BoundedText<Capacity>& value) {
    std::uint8_t length = 0;
    if (!byte(length) || length > Capacity || length > remaining()) return false;
    char buffer[Capacity + 1]{};
    if (length != 0) {
      std::memcpy(buffer, bytes_ + offset_, length);
    }
    offset_ += length;
    return value.assign(buffer);
  }

  bool done() const { return offset_ == length_; }
  std::size_t remaining() const { return length_ - offset_; }

 private:
  const std::uint8_t* bytes_;
  std::size_t length_;
  std::size_t offset_{0};
};

template <typename Enum>
bool decodeEnum(std::uint8_t raw, std::uint8_t maximum, Enum& value) {
  if (raw > maximum) return false;
  value = static_cast<Enum>(raw);
  return true;
}

inline bool decodeBlePayload(const std::uint8_t* bytes, std::size_t length,
                             DecodedBlePayload& decoded) {
  if (bytes == nullptr || length < 2) return false;
  decoded = DecodedBlePayload{};
  BlePayloadReader reader(bytes, length);
  std::uint8_t version = 0, rawType = 0;
  if (!reader.byte(version) || version != 1 || !reader.byte(rawType) ||
      rawType < static_cast<std::uint8_t>(BlePayloadRecordType::SnapshotMetadata) ||
      rawType > static_cast<std::uint8_t>(BlePayloadRecordType::Sample)) {
    return false;
  }
  decoded.recordType = static_cast<BlePayloadRecordType>(rawType);

  if (decoded.recordType == BlePayloadRecordType::SnapshotMetadata) {
    std::uint8_t raw = 0, hasIdentity = 0, hasRegistry = 0;
    if (!reader.byte(raw) ||
        !decodeEnum(raw, 3, decoded.selectionState) ||
        !reader.byte(hasIdentity) || hasIdentity > 1) {
      return false;
    }
    decoded.hasActiveIdentity = hasIdentity != 0;
    if (decoded.hasActiveIdentity && !reader.u32(decoded.activeIdentity)) {
      return false;
    }
    if (!reader.byte(hasRegistry) || hasRegistry > 1) return false;
    decoded.hasCapabilityRegistry = hasRegistry != 0;
    if (!reader.u16(decoded.capabilityCount) || !reader.u16(decoded.sampleCount) ||
        ((decoded.selectionState == ProfileSelectionState::Selected) !=
         decoded.hasActiveIdentity) ||
        (!decoded.hasCapabilityRegistry && decoded.capabilityCount != 0)) {
      return false;
    }
    return reader.done();
  }

  if (decoded.recordType == BlePayloadRecordType::Capability) {
    std::uint16_t index = 0;
    std::uint8_t raw = 0;
    if (!reader.u16(index) || !reader.string(decoded.capability.identity) ||
        !reader.byte(raw) || !decodeEnum(raw, 3, decoded.capability.expectedValueType) ||
        !reader.string(decoded.capability.unit) || !reader.byte(raw) ||
        !decodeEnum(raw, 3, decoded.capability.support) || !reader.done()) {
      return false;
    }
    decoded.index = index;
    return true;
  }

  std::uint16_t index = 0;
  std::uint8_t raw = 0, present = 0;
  if (!reader.u16(index) || !reader.string(decoded.signalId) || !reader.byte(raw) ||
      !decodeEnum(raw, 3, decoded.valueType) || !reader.byte(present) ||
      present > 1) {
    return false;
  }
  decoded.index = index;
  decoded.hasValue = present != 0;
  if (decoded.hasValue) {
    switch (decoded.valueType) {
      case VehicleValueType::Numeric: {
        std::uint64_t bits = 0;
        if (!reader.u64(bits)) return false;
        std::memcpy(&decoded.numericValue, &bits, sizeof(bits));
        if (!std::isfinite(decoded.numericValue)) return false;
        break;
      }
      case VehicleValueType::Boolean:
        if (!reader.byte(raw) || raw > 1) return false;
        decoded.booleanValue = raw != 0;
        break;
      case VehicleValueType::Text:
        if (!reader.string(decoded.textValue)) return false;
        break;
      case VehicleValueType::None:
        return false;
    }
  }
  if (!reader.string(decoded.unit) || !reader.byte(raw) ||
      !decodeEnum(raw, 4, decoded.source) || !reader.byte(raw) ||
      !decodeEnum(raw, 2, decoded.quality) || !reader.byte(raw) ||
      !decodeEnum(raw, 4, decoded.availability) || !reader.u64(decoded.timestampMs) ||
      !reader.done() ||
      (decoded.availability != VehicleAvailability::Available && decoded.hasValue) ||
      (decoded.availability == VehicleAvailability::Available &&
       decoded.valueType == VehicleValueType::None) ||
      (decoded.hasValue && decoded.quality == VehicleQuality::InvalidOrUnknown)) {
    return false;
  }
  return true;
}

}  // namespace vag_data::test
