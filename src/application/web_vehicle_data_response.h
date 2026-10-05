#pragma once

#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

#include "vehicle_data_read_model.h"

namespace vag_data {

enum class WebResponseSerializationStatus : std::uint8_t {
  Serialized,
  InsufficientCapacity,
  InvalidInput,
};

struct WebResponseSerializationResult {
  WebResponseSerializationStatus status;
  // Includes the terminating NUL byte.
  std::size_t requiredCapacity;
};

namespace web_response_detail {

class JsonWriter {
 public:
  JsonWriter(char* output, std::size_t capacity)
      : output_(output), capacity_(capacity) {}

  bool append(const char* text, std::size_t length) {
    if (length > std::numeric_limits<std::size_t>::max() - size_) {
      return false;
    }
    if (output_ != nullptr && length > capacity_ - size_) {
      return false;
    }
    if (output_ != nullptr) {
      for (std::size_t index = 0; index < length; ++index) {
        output_[size_ + index] = text[index];
      }
    }
    size_ += length;
    return true;
  }

  bool appendLiteral(const char* text) {
    std::size_t length = 0;
    while (text[length] != '\0') {
      ++length;
    }
    return append(text, length);
  }

  bool appendQuoted(const char* text) {
    static constexpr char kHex[] = "0123456789abcdef";
    if (!appendLiteral("\"")) {
      return false;
    }
    for (const auto* cursor = reinterpret_cast<const unsigned char*>(text);
         *cursor != '\0'; ++cursor) {
      const char value = static_cast<char>(*cursor);
      switch (*cursor) {
        case '"':
          if (!appendLiteral("\\\"")) return false;
          break;
        case '\\':
          if (!appendLiteral("\\\\")) return false;
          break;
        case '\b':
          if (!appendLiteral("\\b")) return false;
          break;
        case '\f':
          if (!appendLiteral("\\f")) return false;
          break;
        case '\n':
          if (!appendLiteral("\\n")) return false;
          break;
        case '\r':
          if (!appendLiteral("\\r")) return false;
          break;
        case '\t':
          if (!appendLiteral("\\t")) return false;
          break;
        default:
          if (*cursor < 0x20) {
            const char escaped[] = {'\\', 'u', '0', '0',
                                    kHex[*cursor >> 4],
                                    kHex[*cursor & 0x0F]};
            if (!append(escaped, sizeof(escaped))) return false;
          } else if (!append(&value, 1)) {
            return false;
          }
      }
    }
    return appendLiteral("\"");
  }

  bool appendUnsigned(std::uint64_t value) {
    char digits[20];
    const auto converted = std::to_chars(digits, digits + sizeof(digits), value);
    return converted.ec == std::errc{} &&
           append(digits, static_cast<std::size_t>(converted.ptr - digits));
  }

  bool appendNumber(double value) {
    if (!std::isfinite(value)) {
      return false;
    }
    char digits[32];
    const auto converted = std::to_chars(
        digits, digits + sizeof(digits), value, std::chars_format::general,
        std::numeric_limits<double>::max_digits10);
    return converted.ec == std::errc{} &&
           append(digits, static_cast<std::size_t>(converted.ptr - digits));
  }

  std::size_t size() const { return size_; }

 private:
  char* output_;
  std::size_t capacity_;
  std::size_t size_{0};
};

inline const char* selectionStateName(ProfileSelectionState state) {
  switch (state) {
    case ProfileSelectionState::Unknown: return "unknown";
    case ProfileSelectionState::Ambiguous: return "ambiguous";
    case ProfileSelectionState::ManualSelectionRequired:
      return "manual_selection_required";
    case ProfileSelectionState::Selected: return "selected";
  }
  return nullptr;
}

inline const char* valueTypeName(VehicleValueType type) {
  switch (type) {
    case VehicleValueType::None: return "none";
    case VehicleValueType::Numeric: return "numeric";
    case VehicleValueType::Boolean: return "boolean";
    case VehicleValueType::Text: return "text";
  }
  return nullptr;
}

inline const char* supportName(CapabilitySupport support) {
  switch (support) {
    case CapabilitySupport::Supported: return "supported";
    case CapabilitySupport::Unsupported: return "unsupported";
    case CapabilitySupport::Pending: return "pending";
    case CapabilitySupport::Unknown: return "unknown";
  }
  return nullptr;
}

inline const char* availabilityName(VehicleAvailability availability) {
  switch (availability) {
    case VehicleAvailability::Available: return "available";
    case VehicleAvailability::Unsupported: return "unsupported";
    case VehicleAvailability::Unavailable: return "unavailable";
    case VehicleAvailability::Pending: return "pending";
    case VehicleAvailability::Unknown: return "unknown";
  }
  return nullptr;
}

inline const char* qualityName(VehicleQuality quality) {
  switch (quality) {
    case VehicleQuality::ValidCurrent: return "valid_current";
    case VehicleQuality::Stale: return "stale";
    case VehicleQuality::InvalidOrUnknown: return "invalid_or_unknown";
  }
  return nullptr;
}

inline const char* sourceName(VehicleSource source) {
  switch (source) {
    case VehicleSource::Obd: return "obd";
    case VehicleSource::Uds: return "uds";
    case VehicleSource::PassiveCan: return "passive_can";
    case VehicleSource::Derived: return "derived";
    case VehicleSource::Unknown: return "unknown";
  }
  return nullptr;
}

inline bool validateInput(const VehicleDataReadSnapshot& metadata,
                          const NormalizedSignalDescriptor* capabilities,
                          const NormalizedVehicleSample* samples) {
  if (selectionStateName(metadata.selectionState) == nullptr ||
      (metadata.selectionState == ProfileSelectionState::Selected) !=
          metadata.activeIdentity.has_value() ||
      (!metadata.hasCapabilityRegistry && metadata.capabilityCount != 0) ||
      (metadata.capabilityCount != 0 && capabilities == nullptr) ||
      (metadata.sampleCount != 0 && samples == nullptr)) {
    return false;
  }
  for (std::size_t index = 0; index < metadata.capabilityCount; ++index) {
    if (valueTypeName(capabilities[index].expectedValueType) == nullptr ||
        supportName(capabilities[index].support) == nullptr) {
      return false;
    }
  }
  for (std::size_t index = 0; index < metadata.sampleCount; ++index) {
    const auto& sample = samples[index];
    if (availabilityName(sample.availability()) == nullptr ||
        qualityName(sample.quality()) == nullptr ||
        sourceName(sample.source()) == nullptr ||
        valueTypeName(sample.valueType()) == nullptr) {
      return false;
    }
    if (sample.hasValue() && sample.valueType() == VehicleValueType::Numeric &&
        !std::isfinite(sample.value().numericValue())) {
      return false;
    }
  }
  return true;
}

inline bool writeResponse(JsonWriter& json,
                          const VehicleDataReadSnapshot& metadata,
                          const NormalizedSignalDescriptor* capabilities,
                          const NormalizedVehicleSample* samples) {
  if (!json.appendLiteral("{\"profileSelection\":" ) ||
      !json.appendQuoted(selectionStateName(metadata.selectionState))) {
    return false;
  }
  if (metadata.activeIdentity.has_value()) {
    if (!json.appendLiteral(",\"activeProfileId\":" ) ||
        !json.appendUnsigned(metadata.activeIdentity->value())) {
      return false;
    }
  }
  if (!json.appendLiteral(",\"capabilities\":")) {
    return false;
  }
  if (!metadata.hasCapabilityRegistry) {
    if (!json.appendLiteral("null")) return false;
  } else {
    if (!json.appendLiteral("[")) return false;
    for (std::size_t index = 0; index < metadata.capabilityCount; ++index) {
      const auto& capability = capabilities[index];
      if ((index != 0 && !json.appendLiteral(",")) ||
          !json.appendLiteral("{\"signalId\":" ) ||
          !json.appendQuoted(capability.identity.c_str()) ||
          !json.appendLiteral(",\"valueType\":" ) ||
          !json.appendQuoted(valueTypeName(capability.expectedValueType)) ||
          !json.appendLiteral(",\"unit\":" ) ||
          !json.appendQuoted(capability.unit.c_str()) ||
          !json.appendLiteral(",\"support\":" ) ||
          !json.appendQuoted(supportName(capability.support)) ||
          !json.appendLiteral("}")) {
        return false;
      }
    }
    if (!json.appendLiteral("]")) return false;
  }
  if (!json.appendLiteral(",\"samples\":[")) return false;
  for (std::size_t index = 0; index < metadata.sampleCount; ++index) {
    const auto& sample = samples[index];
    if ((index != 0 && !json.appendLiteral(",")) ||
        !json.appendLiteral("{\"signalId\":" ) ||
        !json.appendQuoted(sample.signalId().c_str()) ||
        !json.appendLiteral(",\"unit\":" ) ||
        !json.appendQuoted(sample.unit().c_str())) {
      return false;
    }
    if (sample.hasValue()) {
      if (!json.appendLiteral(",\"valueType\":" ) ||
          !json.appendQuoted(valueTypeName(sample.valueType())) ||
          !json.appendLiteral(",\"value\":")) {
        return false;
      }
      switch (sample.valueType()) {
        case VehicleValueType::Numeric:
          if (!json.appendNumber(sample.value().numericValue())) return false;
          break;
        case VehicleValueType::Boolean:
          if (!json.appendLiteral(sample.value().booleanValue() ? "true" : "false"))
            return false;
          break;
        case VehicleValueType::Text:
          if (!json.appendQuoted(sample.value().textValue().c_str())) return false;
          break;
        case VehicleValueType::None:
          return false;
      }
    }
    if (!json.appendLiteral(",\"source\":" ) ||
        !json.appendQuoted(sourceName(sample.source())) ||
        !json.appendLiteral(",\"quality\":" ) ||
        !json.appendQuoted(qualityName(sample.quality())) ||
        !json.appendLiteral(",\"availability\":" ) ||
        !json.appendQuoted(availabilityName(sample.availability())) ||
        !json.appendLiteral(",\"timestampMs\":\"" ) ||
        !json.appendUnsigned(sample.timestampMs()) ||
        !json.appendLiteral("\"}")) {
      return false;
    }
  }
  return json.appendLiteral("]}");
}

}  // namespace web_response_detail

// Serializes only the copied output of VehicleDataReadModel. A sizing pass makes
// insufficient-capacity failures explicit and leaves the caller buffer intact.
inline WebResponseSerializationResult serializeWebVehicleDataResponse(
    const VehicleDataReadSnapshot& metadata,
    const NormalizedSignalDescriptor* capabilities,
    const NormalizedVehicleSample* samples, char* output,
    std::size_t outputCapacity) {
  using namespace web_response_detail;
  if (!validateInput(metadata, capabilities, samples)) {
    return {WebResponseSerializationStatus::InvalidInput, 0};
  }

  JsonWriter sizing(nullptr, 0);
  if (!writeResponse(sizing, metadata, capabilities, samples) ||
      sizing.size() == std::numeric_limits<std::size_t>::max()) {
    return {WebResponseSerializationStatus::InvalidInput, 0};
  }
  const std::size_t requiredCapacity = sizing.size() + 1;
  if (outputCapacity < requiredCapacity) {
    return {WebResponseSerializationStatus::InsufficientCapacity,
            requiredCapacity};
  }
  if (output == nullptr) {
    return {WebResponseSerializationStatus::InvalidInput, requiredCapacity};
  }

  JsonWriter writer(output, outputCapacity);
  if (!writeResponse(writer, metadata, capabilities, samples) ||
      writer.size() != sizing.size()) {
    return {WebResponseSerializationStatus::InvalidInput, requiredCapacity};
  }
  output[writer.size()] = '\0';
  return {WebResponseSerializationStatus::Serialized, requiredCapacity};
}

}  // namespace vag_data
