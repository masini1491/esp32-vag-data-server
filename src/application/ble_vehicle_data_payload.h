#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

#include "vehicle_data_read_model.h"

namespace vag_data {

enum class BlePayloadStatus : std::uint8_t {
  Encoded,
  InsufficientCapacity,
  InvalidInput,
};

struct BlePayloadResult {
  BlePayloadStatus status;
  std::size_t requiredCapacity;
};

// Logical protocol v1 (not a GATT definition): all records start with
// version:u8 (=1), type:u8 (snapshot=1, capability=2, sample=3). Integers and
// IEEE-754 binary64 values use network byte order; strings are length-prefixed
// byte sequences. No BLE stack, ATT/MTU or dynamic-allocation policy is implied.
// Snapshot: selection:u8 (Unknown=0, Ambiguous=1, Manual=2, Selected=3),
// identity-present:u8, optional identity:u32, registry-present:u8,
// capability-count:u16, sample-count:u16.
// Capability: index:u16, signal-id:string, value-type:u8 (None=0, Numeric=1,
// Boolean=2, Text=3), unit:string, support:u8 (Supported=0, Unsupported=1,
// Pending=2, Unknown=3).
// Sample: index:u16, signal-id:string, value-type:u8, value-present:u8,
// optional typed value (binary64 / bool:u8 / text), unit:string, source:u8
// (OBD=0, UDS=1, PassiveCAN=2, Derived=3, Unknown=4), quality:u8
// (ValidCurrent=0, Stale=1, InvalidOrUnknown=2), availability:u8
// (Available=0, Unsupported=1, Unavailable=2, Pending=3, Unknown=4),
// timestamp-ms:u64. All multibyte integers are unsigned and big-endian.
enum class BlePayloadRecordType : std::uint8_t {
  SnapshotMetadata = 1,
  Capability = 2,
  Sample = 3,
};

namespace ble_payload_detail {

class Writer {
 public:
  Writer(std::uint8_t* output, std::size_t capacity)
      : output_(output), capacity_(capacity) {}

  bool appendByte(std::uint8_t value) { return append(&value, 1); }

  bool append(const std::uint8_t* bytes, std::size_t length) {
    if (length > std::numeric_limits<std::size_t>::max() - size_) {
      return false;
    }
    if (output_ != nullptr && (size_ > capacity_ || length > capacity_ - size_)) {
      return false;
    }
    if (output_ != nullptr) {
      std::memcpy(output_ + size_, bytes, length);
    }
    size_ += length;
    return true;
  }

  bool appendText(const char* text, std::size_t length) {
    return append(reinterpret_cast<const std::uint8_t*>(text), length);
  }

  bool appendU16(std::uint16_t value) {
    return appendByte(static_cast<std::uint8_t>(value >> 8)) &&
           appendByte(static_cast<std::uint8_t>(value));
  }

  bool appendU32(std::uint32_t value) {
    for (int shift = 24; shift >= 0; shift -= 8) {
      if (!appendByte(static_cast<std::uint8_t>(value >> shift))) return false;
    }
    return true;
  }

  bool appendU64(std::uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8) {
      if (!appendByte(static_cast<std::uint8_t>(value >> shift))) return false;
    }
    return true;
  }

  bool appendHeader(BlePayloadRecordType type) {
    return appendByte(1) && appendByte(static_cast<std::uint8_t>(type));
  }

  bool appendString(const char* text, std::size_t length) {
    return length <= std::numeric_limits<std::uint8_t>::max() &&
           appendByte(static_cast<std::uint8_t>(length)) &&
           appendText(text, length);
  }

  bool appendDouble(double value) {
    static_assert(sizeof(double) == sizeof(std::uint64_t),
                  "BLE payload v1 requires binary64 double");
    static_assert(std::numeric_limits<double>::is_iec559,
                  "BLE payload v1 requires IEEE-754 double");
    std::uint64_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));
    return appendU64(bits);
  }

  std::size_t size() const { return size_; }

 private:
  std::uint8_t* output_;
  std::size_t capacity_;
  std::size_t size_{0};
};

inline bool valid(ProfileSelectionState state) {
  return state == ProfileSelectionState::Unknown ||
         state == ProfileSelectionState::Ambiguous ||
         state == ProfileSelectionState::ManualSelectionRequired ||
         state == ProfileSelectionState::Selected;
}

inline bool valid(VehicleValueType type) {
  return type == VehicleValueType::None || type == VehicleValueType::Numeric ||
         type == VehicleValueType::Boolean || type == VehicleValueType::Text;
}

inline bool valid(CapabilitySupport support) {
  return support == CapabilitySupport::Supported ||
         support == CapabilitySupport::Unsupported ||
         support == CapabilitySupport::Pending ||
         support == CapabilitySupport::Unknown;
}

inline bool valid(VehicleAvailability availability) {
  return availability == VehicleAvailability::Available ||
         availability == VehicleAvailability::Unsupported ||
         availability == VehicleAvailability::Unavailable ||
         availability == VehicleAvailability::Pending ||
         availability == VehicleAvailability::Unknown;
}

inline bool valid(VehicleQuality quality) {
  return quality == VehicleQuality::ValidCurrent ||
         quality == VehicleQuality::Stale ||
         quality == VehicleQuality::InvalidOrUnknown;
}

inline bool valid(VehicleSource source) {
  return source == VehicleSource::Obd || source == VehicleSource::Uds ||
         source == VehicleSource::PassiveCan || source == VehicleSource::Derived ||
         source == VehicleSource::Unknown;
}

inline std::uint8_t code(ProfileSelectionState value) {
  switch (value) {
    case ProfileSelectionState::Unknown: return 0;
    case ProfileSelectionState::Ambiguous: return 1;
    case ProfileSelectionState::ManualSelectionRequired: return 2;
    case ProfileSelectionState::Selected: return 3;
  }
  return 0xFF;
}
inline std::uint8_t code(VehicleValueType value) {
  switch (value) {
    case VehicleValueType::None: return 0;
    case VehicleValueType::Numeric: return 1;
    case VehicleValueType::Boolean: return 2;
    case VehicleValueType::Text: return 3;
  }
  return 0xFF;
}
inline std::uint8_t code(CapabilitySupport value) {
  switch (value) {
    case CapabilitySupport::Supported: return 0;
    case CapabilitySupport::Unsupported: return 1;
    case CapabilitySupport::Pending: return 2;
    case CapabilitySupport::Unknown: return 3;
  }
  return 0xFF;
}
inline std::uint8_t code(VehicleAvailability value) {
  switch (value) {
    case VehicleAvailability::Available: return 0;
    case VehicleAvailability::Unsupported: return 1;
    case VehicleAvailability::Unavailable: return 2;
    case VehicleAvailability::Pending: return 3;
    case VehicleAvailability::Unknown: return 4;
  }
  return 0xFF;
}
inline std::uint8_t code(VehicleQuality value) {
  switch (value) {
    case VehicleQuality::ValidCurrent: return 0;
    case VehicleQuality::Stale: return 1;
    case VehicleQuality::InvalidOrUnknown: return 2;
  }
  return 0xFF;
}
inline std::uint8_t code(VehicleSource value) {
  switch (value) {
    case VehicleSource::Obd: return 0;
    case VehicleSource::Uds: return 1;
    case VehicleSource::PassiveCan: return 2;
    case VehicleSource::Derived: return 3;
    case VehicleSource::Unknown: return 4;
  }
  return 0xFF;
}

inline bool write(Writer& writer, const VehicleDataReadSnapshot& snapshot) {
  constexpr auto maxCount = std::numeric_limits<std::uint16_t>::max();
  if (!valid(snapshot.selectionState) ||
      ((snapshot.selectionState == ProfileSelectionState::Selected) !=
       snapshot.activeIdentity.has_value()) ||
      (!snapshot.hasCapabilityRegistry && snapshot.capabilityCount != 0) ||
      snapshot.capabilityCount > maxCount || snapshot.sampleCount > maxCount) {
    return false;
  }
  return writer.appendHeader(BlePayloadRecordType::SnapshotMetadata) &&
         writer.appendByte(code(snapshot.selectionState)) &&
         writer.appendByte(snapshot.activeIdentity.has_value() ? 1 : 0) &&
         (!snapshot.activeIdentity.has_value() ||
          writer.appendU32(snapshot.activeIdentity->value())) &&
         writer.appendByte(snapshot.hasCapabilityRegistry ? 1 : 0) &&
         writer.appendU16(static_cast<std::uint16_t>(snapshot.capabilityCount)) &&
         writer.appendU16(static_cast<std::uint16_t>(snapshot.sampleCount));
}

inline bool write(Writer& writer, const NormalizedSignalDescriptor& capability,
                  std::size_t index) {
  constexpr auto maxIndex = std::numeric_limits<std::uint16_t>::max();
  if (index > maxIndex || !valid(capability.expectedValueType) ||
      !valid(capability.support)) {
    return false;
  }
  return writer.appendHeader(BlePayloadRecordType::Capability) &&
         writer.appendU16(static_cast<std::uint16_t>(index)) &&
         writer.appendString(capability.identity.c_str(), capability.identity.size()) &&
         writer.appendByte(code(capability.expectedValueType)) &&
         writer.appendString(capability.unit.c_str(), capability.unit.size()) &&
         writer.appendByte(code(capability.support));
}

inline bool write(Writer& writer, const NormalizedVehicleSample& sample,
                  std::size_t index) {
  constexpr auto maxIndex = std::numeric_limits<std::uint16_t>::max();
  const auto type = sample.valueType();
  const bool hasValue = sample.hasValue();
  if (index > maxIndex || !valid(type) || !valid(sample.source()) ||
      !valid(sample.quality()) || !valid(sample.availability()) ||
      (sample.availability() != VehicleAvailability::Available && hasValue) ||
      (sample.availability() == VehicleAvailability::Available &&
       type == VehicleValueType::None) ||
      (hasValue && type == VehicleValueType::None) ||
      (hasValue && type == VehicleValueType::Numeric &&
       !std::isfinite(sample.value().numericValue()))) {
    return false;
  }

  if (!writer.appendHeader(BlePayloadRecordType::Sample) ||
      !writer.appendU16(static_cast<std::uint16_t>(index)) ||
      !writer.appendString(sample.signalId().c_str(), sample.signalId().size()) ||
      !writer.appendByte(code(type)) || !writer.appendByte(hasValue ? 1 : 0)) {
    return false;
  }
  if (hasValue) {
    switch (type) {
      case VehicleValueType::Numeric:
        if (!writer.appendDouble(sample.value().numericValue())) return false;
        break;
      case VehicleValueType::Boolean:
        if (!writer.appendByte(sample.value().booleanValue() ? 1 : 0)) return false;
        break;
      case VehicleValueType::Text:
        if (!writer.appendString(sample.value().textValue().c_str(),
                                  sample.value().textValue().size())) {
          return false;
        }
        break;
      case VehicleValueType::None:
        return false;
    }
  }
  return writer.appendString(sample.unit().c_str(), sample.unit().size()) &&
         writer.appendByte(code(sample.source())) &&
         writer.appendByte(code(sample.quality())) &&
         writer.appendByte(code(sample.availability())) &&
         writer.appendU64(sample.timestampMs());
}

template <typename T, bool (*Write)(Writer&, const T&)>
BlePayloadResult encode(const T& value, std::uint8_t* output,
                        std::size_t capacity) {
  Writer sizing(nullptr, 0);
  if (!Write(sizing, value)) return {BlePayloadStatus::InvalidInput, 0};
  const std::size_t required = sizing.size();
  if (capacity < required) {
    return {BlePayloadStatus::InsufficientCapacity, required};
  }
  if (output == nullptr) return {BlePayloadStatus::InvalidInput, required};
  Writer writer(output, capacity);
  if (!Write(writer, value) || writer.size() != required) {
    return {BlePayloadStatus::InvalidInput, required};
  }
  return {BlePayloadStatus::Encoded, required};
}

struct CapabilityInput {
  const NormalizedSignalDescriptor& value;
  std::size_t index;
};
inline bool write(Writer& writer, const CapabilityInput& input) {
  return write(writer, input.value, input.index);
}

struct SampleInput {
  const NormalizedVehicleSample& value;
  std::size_t index;
};
inline bool write(Writer& writer, const SampleInput& input) {
  return write(writer, input.value, input.index);
}

}  // namespace ble_payload_detail

inline BlePayloadResult encodeBleSnapshotMetadata(
    const VehicleDataReadSnapshot& snapshot, std::uint8_t* output,
    std::size_t capacity) {
  return ble_payload_detail::encode<VehicleDataReadSnapshot,
                                    ble_payload_detail::write>(snapshot, output,
                                                              capacity);
}

inline BlePayloadResult encodeBleCapabilityRecord(
    const NormalizedSignalDescriptor& capability, std::size_t index,
    std::uint8_t* output, std::size_t capacity) {
  const ble_payload_detail::CapabilityInput input{capability, index};
  return ble_payload_detail::encode<ble_payload_detail::CapabilityInput,
                                    ble_payload_detail::write>(input, output,
                                                              capacity);
}

inline BlePayloadResult encodeBleSampleRecord(
    const NormalizedVehicleSample& sample, std::size_t index,
    std::uint8_t* output, std::size_t capacity) {
  const ble_payload_detail::SampleInput input{sample, index};
  return ble_payload_detail::encode<ble_payload_detail::SampleInput,
                                    ble_payload_detail::write>(input, output,
                                                              capacity);
}

}  // namespace vag_data
