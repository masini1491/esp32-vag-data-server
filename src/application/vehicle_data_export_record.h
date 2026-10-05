#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

#include "../core/vehicle_data.h"

namespace vag_data {

enum class VehicleDataExportStatus : std::uint8_t {
  Ok,
  BufferTooSmall,
  InvalidArgument,
  InvalidSample,
  MalformedRecord,
  UnsupportedVersion,
};

class VehicleDataExportRecord {
 public:
  static constexpr std::uint8_t kVersion = 1;
  static constexpr std::size_t kHeaderSize = 23;

  static VehicleDataExportStatus requiredCapacity(
      const NormalizedVehicleSample& sample, std::size_t& required) {
    required = 0;
    if (!isValid(sample)) {
      return VehicleDataExportStatus::InvalidSample;
    }
    required = kHeaderSize + sample.signalId().size() + sample.unit().size() +
               payloadLength(sample);
    return VehicleDataExportStatus::Ok;
  }

  static VehicleDataExportStatus encode(
      const NormalizedVehicleSample& sample, std::uint8_t* output,
      std::size_t capacity, std::size_t& required) {
    const auto status = requiredCapacity(sample, required);
    if (status != VehicleDataExportStatus::Ok) {
      return status;
    }
    if (capacity < required) {
      return VehicleDataExportStatus::BufferTooSmall;
    }
    if (output == nullptr) {
      return VehicleDataExportStatus::InvalidArgument;
    }

    output[0] = 'V';
    output[1] = 'D';
    output[2] = 'E';
    output[3] = 'R';
    output[4] = kVersion;
    writeU16(output + 5, static_cast<std::uint16_t>(required));
    output[7] = static_cast<std::uint8_t>(sample.valueType());
    output[8] = sample.hasValue() ? 1 : 0;
    output[9] = static_cast<std::uint8_t>(sample.availability());
    output[10] = static_cast<std::uint8_t>(sample.quality());
    output[11] = static_cast<std::uint8_t>(sample.source());
    writeU64(output + 12, sample.timestampMs());
    output[20] = static_cast<std::uint8_t>(sample.signalId().size());
    output[21] = static_cast<std::uint8_t>(sample.unit().size());
    output[22] = static_cast<std::uint8_t>(payloadLength(sample));

    auto* cursor = output + kHeaderSize;
    copyBytes(cursor, sample.signalId().c_str(), sample.signalId().size());
    cursor += sample.signalId().size();
    copyBytes(cursor, sample.unit().c_str(), sample.unit().size());
    cursor += sample.unit().size();
    writePayload(sample, cursor);
    return VehicleDataExportStatus::Ok;
  }

  static VehicleDataExportStatus decode(
      const std::uint8_t* input, std::size_t length,
      NormalizedVehicleSample& output) {
    if (input == nullptr) {
      return VehicleDataExportStatus::InvalidArgument;
    }
    if (length < kHeaderSize || input[0] != 'V' || input[1] != 'D' ||
        input[2] != 'E' || input[3] != 'R' ||
        readU16(input + 5) != length) {
      return VehicleDataExportStatus::MalformedRecord;
    }
    if (input[4] != kVersion) {
      return VehicleDataExportStatus::UnsupportedVersion;
    }

    const auto type = static_cast<VehicleValueType>(input[7]);
    const auto hasValue = input[8];
    const auto availability = static_cast<VehicleAvailability>(input[9]);
    const auto quality = static_cast<VehicleQuality>(input[10]);
    const auto source = static_cast<VehicleSource>(input[11]);
    const auto signalLength = static_cast<std::size_t>(input[20]);
    const auto unitLength = static_cast<std::size_t>(input[21]);
    const auto valueLength = static_cast<std::size_t>(input[22]);
    const auto expectedLength = kHeaderSize + signalLength + unitLength +
                                valueLength;

    if (!isValidType(type) || hasValue > 1 ||
        !isValidAvailability(availability) || !isValidQuality(quality) ||
        !isValidSource(source) ||
        signalLength > NormalizedVehicleSample::kMaxSignalIdLength ||
        unitLength > NormalizedVehicleSample::kMaxUnitLength ||
        expectedLength != length) {
      return VehicleDataExportStatus::MalformedRecord;
    }

    const auto* signal = input + kHeaderSize;
    const auto* unit = signal + signalLength;
    const auto* value = unit + unitLength;
    if (containsNull(signal, signalLength) || containsNull(unit, unitLength)) {
      return VehicleDataExportStatus::MalformedRecord;
    }

    char signalBuffer[NormalizedVehicleSample::kMaxSignalIdLength + 1]{};
    char unitBuffer[NormalizedVehicleSample::kMaxUnitLength + 1]{};
    copyBytes(reinterpret_cast<std::uint8_t*>(signalBuffer), signal,
              signalLength);
    copyBytes(reinterpret_cast<std::uint8_t*>(unitBuffer), unit, unitLength);

    NormalizedVehicleSample candidate;
    const auto timestamp = readU64(input + 12);
    bool constructed = false;
    switch (type) {
      case VehicleValueType::Numeric: {
        if (valueLength != sizeof(std::uint64_t)) {
          return VehicleDataExportStatus::MalformedRecord;
        }
        const auto bits = readU64(value);
        double numeric = 0.0;
        static_assert(sizeof(numeric) == sizeof(bits),
                      "Logger export requires 64-bit double");
        static_assert(std::numeric_limits<double>::is_iec559,
                      "Logger export requires IEC 60559 double");
        std::memcpy(&numeric, &bits, sizeof(numeric));
        if (!std::isfinite(numeric)) {
          return VehicleDataExportStatus::MalformedRecord;
        }
        constructed = NormalizedVehicleSample::tryNumeric(
            signalBuffer, numeric, unitBuffer, source, quality, timestamp,
            candidate);
        break;
      }
      case VehicleValueType::Boolean:
        if (valueLength != 1 || value[0] > 1) {
          return VehicleDataExportStatus::MalformedRecord;
        }
        constructed = NormalizedVehicleSample::tryBoolean(
            signalBuffer, value[0] != 0, unitBuffer, source, quality,
            timestamp, candidate);
        break;
      case VehicleValueType::Text:
        if (valueLength > NormalizedVehicleValue::kMaxTextLength ||
            containsNull(value, valueLength)) {
          return VehicleDataExportStatus::MalformedRecord;
        }
        {
          char textBuffer[NormalizedVehicleValue::kMaxTextLength + 1]{};
          copyBytes(reinterpret_cast<std::uint8_t*>(textBuffer), value,
                    valueLength);
          constructed = NormalizedVehicleSample::tryText(
              signalBuffer, textBuffer, unitBuffer, source, quality, timestamp,
              candidate);
        }
        break;
      case VehicleValueType::None:
        if (availability == VehicleAvailability::Available ||
            valueLength != 0) {
          return VehicleDataExportStatus::MalformedRecord;
        }
        constructed = NormalizedVehicleSample::tryStatus(
            signalBuffer, availability, unitBuffer, source, quality, timestamp,
            candidate);
        break;
    }

    if (!constructed || candidate.availability() != availability ||
        candidate.hasValue() != (hasValue == 1)) {
      return VehicleDataExportStatus::MalformedRecord;
    }
    output = candidate;
    return VehicleDataExportStatus::Ok;
  }

 private:
  static bool isValidType(VehicleValueType type) {
    return type == VehicleValueType::None ||
           type == VehicleValueType::Numeric ||
           type == VehicleValueType::Boolean || type == VehicleValueType::Text;
  }

  static bool isValidAvailability(VehicleAvailability value) {
    return value == VehicleAvailability::Available ||
           value == VehicleAvailability::Unsupported ||
           value == VehicleAvailability::Unavailable ||
           value == VehicleAvailability::Pending ||
           value == VehicleAvailability::Unknown;
  }

  static bool isValidQuality(VehicleQuality value) {
    return value == VehicleQuality::ValidCurrent ||
           value == VehicleQuality::Stale ||
           value == VehicleQuality::InvalidOrUnknown;
  }

  static bool isValidSource(VehicleSource value) {
    return value == VehicleSource::Obd || value == VehicleSource::Uds ||
           value == VehicleSource::PassiveCan ||
           value == VehicleSource::Derived || value == VehicleSource::Unknown;
  }

  static bool isValid(const NormalizedVehicleSample& sample) {
    if (sample.signalId().size() > NormalizedVehicleSample::kMaxSignalIdLength ||
        sample.unit().size() > NormalizedVehicleSample::kMaxUnitLength ||
        !isValidAvailability(sample.availability()) ||
        !isValidQuality(sample.quality()) || !isValidSource(sample.source())) {
      return false;
    }
    switch (sample.valueType()) {
      case VehicleValueType::Numeric:
        return sample.availability() == VehicleAvailability::Available &&
               std::isfinite(sample.value().numericValue());
      case VehicleValueType::Boolean:
        return sample.availability() == VehicleAvailability::Available;
      case VehicleValueType::Text:
        return sample.availability() == VehicleAvailability::Available &&
               sample.value().textValue().size() <=
                   NormalizedVehicleValue::kMaxTextLength;
      case VehicleValueType::None:
        return sample.availability() != VehicleAvailability::Available &&
               !sample.hasValue();
    }
    return false;
  }

  static std::size_t payloadLength(const NormalizedVehicleSample& sample) {
    switch (sample.valueType()) {
      case VehicleValueType::Numeric:
        return sizeof(std::uint64_t);
      case VehicleValueType::Boolean:
        return 1;
      case VehicleValueType::Text:
        return sample.value().textValue().size();
      case VehicleValueType::None:
        return 0;
    }
    return 0;
  }

  static void writePayload(const NormalizedVehicleSample& sample,
                           std::uint8_t* output) {
    switch (sample.valueType()) {
      case VehicleValueType::Numeric: {
        std::uint64_t bits = 0;
        const auto numeric = sample.value().numericValue();
        std::memcpy(&bits, &numeric, sizeof(bits));
        writeU64(output, bits);
        break;
      }
      case VehicleValueType::Boolean:
        output[0] = sample.value().booleanValue() ? 1 : 0;
        break;
      case VehicleValueType::Text:
        copyBytes(output, sample.value().textValue().c_str(),
                  sample.value().textValue().size());
        break;
      case VehicleValueType::None:
        break;
    }
  }

  static bool containsNull(const std::uint8_t* bytes, std::size_t length) {
    for (std::size_t index = 0; index < length; ++index) {
      if (bytes[index] == 0) {
        return true;
      }
    }
    return false;
  }

  static void copyBytes(std::uint8_t* destination, const char* source,
                        std::size_t length) {
    if (length > 0) {
      std::memcpy(destination, source, length);
    }
  }

  static void copyBytes(std::uint8_t* destination,
                        const std::uint8_t* source, std::size_t length) {
    if (length > 0) {
      std::memcpy(destination, source, length);
    }
  }

  static void writeU16(std::uint8_t* output, std::uint16_t value) {
    output[0] = static_cast<std::uint8_t>(value & 0xff);
    output[1] = static_cast<std::uint8_t>((value >> 8) & 0xff);
  }

  static std::uint16_t readU16(const std::uint8_t* input) {
    return static_cast<std::uint16_t>(input[0]) |
           static_cast<std::uint16_t>(input[1] << 8);
  }

  static void writeU64(std::uint8_t* output, std::uint64_t value) {
    for (std::size_t index = 0; index < sizeof(value); ++index) {
      output[index] = static_cast<std::uint8_t>((value >> (index * 8)) & 0xff);
    }
  }

  static std::uint64_t readU64(const std::uint8_t* input) {
    std::uint64_t result = 0;
    for (std::size_t index = 0; index < sizeof(result); ++index) {
      result |= static_cast<std::uint64_t>(input[index]) << (index * 8);
    }
    return result;
  }
};

}  // namespace vag_data
