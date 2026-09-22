#pragma once

#include <cstddef>
#include <cstdint>

#include "diagnostic_transport.h"

namespace vag_data {

enum class ReadOnlyStatus : std::uint8_t {
  Forwarded,
  PolicyDenied,
  InvalidRequest,
};

struct ReadOnlyResult {
  ReadOnlyStatus status;
  TransportStatus transportStatus;
};

// Semantic OBD request shape for the Phase 3A single-PID read-only boundary.
// The guard constructs the diagnostic payload itself; callers cannot submit an
// arbitrary raw diagnostic payload through this API.
struct ObdSinglePidRequest {
  std::uint8_t mode;
  const std::uint8_t* pid;
  std::size_t pidCount;
};

class ReadOnlyGuard {
 public:
  explicit ReadOnlyGuard(DiagnosticTransport& transport) : transport_(transport) {}

  ReadOnlyResult startObdSinglePid(const ObdSinglePidRequest& request) {
    if (request.pid == nullptr || request.pidCount != 1) {
      return {ReadOnlyStatus::InvalidRequest, TransportStatus::Idle};
    }

    if (request.mode != kModeCurrentData &&
        request.mode != kModeVehicleInformation) {
      return {ReadOnlyStatus::PolicyDenied, TransportStatus::Idle};
    }

    const std::uint8_t payload[] = {request.mode, request.pid[0]};
    return {ReadOnlyStatus::Forwarded,
            transport_.startSend(payload, sizeof(payload))};
  }

  TransportStatus poll() { return transport_.poll(); }

  TransportStatus receive(std::uint8_t* payload, std::size_t capacity,
                          std::size_t& length) {
    return transport_.receive(payload, capacity, length);
  }

 private:
  static constexpr std::uint8_t kModeCurrentData = 0x01;
  static constexpr std::uint8_t kModeVehicleInformation = 0x09;

  DiagnosticTransport& transport_;
};

}  // namespace vag_data
