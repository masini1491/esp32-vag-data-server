#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include "../core/clock.h"
#include "read_only_guard.h"

namespace vag_data {

enum class ObdServiceStatus : std::uint8_t {
  Idle,
  InProgress,
  Busy,
  ResponseReady,
  Timeout,
  InvalidResponse,
  UnexpectedResponse,
  PolicyDenied,
  InvalidRequest,
  TransportFailure,
};

struct ObdServiceConfig {
  std::uint32_t responseTimeoutMs{1000};
};

struct ObdDtcRecord {
  std::uint8_t firstByte{0};
  std::uint8_t secondByte{0};
};

class ObdService {
 public:
  static constexpr std::size_t kMaxRawDataLength = 64;
  static constexpr std::size_t kVinLength = 17;
  static constexpr std::size_t kMaxStoredDtcRecords = 16;

  ObdService(ReadOnlyGuard& guard, Clock& clock,
             const ObdServiceConfig& config = {})
      : guard_(guard), clock_(clock), config_(config) {}

  ObdServiceStatus requestCurrentData(std::uint8_t pid) {
    return beginMode01Request(pid, RequestKind::CurrentData);
  }

  ObdServiceStatus requestSupportedPids(std::uint8_t basePid) {
    if (!isSupportedPidBlockBase(basePid)) {
      return finishWithoutTransport(ObdServiceStatus::InvalidRequest);
    }
    return beginMode01Request(basePid, RequestKind::SupportedPids);
  }

  ObdServiceStatus requestVin() {
    const std::uint8_t pid = kVinPid;
    return beginRequest(kModeVehicleInformation, pid, RequestKind::Vin);
  }

  ObdServiceStatus requestStoredDtcs() {
    if (!prepareRequest(RequestKind::StoredDtcs)) {
      return status_;
    }
    return beginTransport(guard_.startObdStoredDtcRead());
  }

  ObdServiceStatus poll() {
    if (state_ == State::Idle) {
      return ObdServiceStatus::Idle;
    }

    const auto pollStatus = guard_.poll();
    lastTransportStatus_ = pollStatus;
    if (state_ == State::AwaitingTransmit) {
      if (pollStatus == TransportStatus::InProgress) {
        return setStatus(ObdServiceStatus::InProgress);
      }
      if (pollStatus == TransportStatus::Complete) {
        beginResponseWait();
        return setStatus(ObdServiceStatus::InProgress);
      }
      return finishTransportFailure();
    }

    if (!isNonFailingPollStatus(pollStatus)) {
      return finishTransportFailure();
    }
    if (clock_.nowMs() >= responseDeadlineMs_) {
      return finish(ObdServiceStatus::Timeout);
    }

    std::size_t responseLength = 0;
    const auto receiveStatus =
        guard_.receive(response_.data(), response_.size(), responseLength);
    lastTransportStatus_ = receiveStatus;
    if (receiveStatus == TransportStatus::NoData) {
      return setStatus(ObdServiceStatus::InProgress);
    }
    if (receiveStatus != TransportStatus::Complete) {
      return finishTransportFailure();
    }
    return handleResponse(responseLength);
  }

  ObdServiceStatus status() const { return status_; }
  bool isRequestActive() const { return state_ != State::Idle; }
  TransportStatus lastTransportStatus() const { return lastTransportStatus_; }
  std::size_t rawDataLength() const { return rawDataLength_; }
  const std::array<std::uint8_t, kMaxRawDataLength>& rawData() const {
    return rawData_;
  }
  std::uint32_t supportedPidBitmap() const { return supportedPidBitmap_; }
  const std::array<char, kVinLength>& vin() const { return vin_; }
  std::size_t storedDtcCount() const { return storedDtcCount_; }
  const std::array<ObdDtcRecord, kMaxStoredDtcRecords>& storedDtcs() const {
    return storedDtcs_;
  }

 private:
  enum class State : std::uint8_t {
    Idle,
    AwaitingTransmit,
    AwaitingResponse,
  };

  enum class RequestKind : std::uint8_t {
    None,
    CurrentData,
    SupportedPids,
    Vin,
    StoredDtcs,
  };

  static constexpr std::uint8_t kModeCurrentData = 0x01;
  static constexpr std::uint8_t kModeVehicleInformation = 0x09;
  static constexpr std::uint8_t kVinPid = 0x02;

  ObdServiceStatus beginMode01Request(std::uint8_t pid, RequestKind kind) {
    return beginRequest(kModeCurrentData, pid, kind);
  }

  ObdServiceStatus beginRequest(std::uint8_t mode, std::uint8_t pid,
                                RequestKind kind) {
    if (!prepareRequest(kind)) {
      return status_;
    }
    requestedPid_ = pid;
    return beginTransport(
        guard_.startObdSinglePid({mode, &requestedPid_, 1}));
  }

  bool prepareRequest(RequestKind kind) {
    if (isRequestActive()) {
      setStatus(ObdServiceStatus::Busy);
      return false;
    }
    if (config_.responseTimeoutMs == 0) {
      finishWithoutTransport(ObdServiceStatus::InvalidRequest);
      return false;
    }

    clearResult();
    requestKind_ = kind;
    return true;
  }

  ObdServiceStatus beginTransport(const ReadOnlyResult& guardResult) {
    lastTransportStatus_ = guardResult.transportStatus;
    if (guardResult.status == ReadOnlyStatus::PolicyDenied) {
      return finishWithoutTransport(ObdServiceStatus::PolicyDenied);
    }
    if (guardResult.status == ReadOnlyStatus::InvalidRequest) {
      return finishWithoutTransport(ObdServiceStatus::InvalidRequest);
    }

    if (guardResult.transportStatus == TransportStatus::Complete) {
      beginResponseWait();
      return setStatus(ObdServiceStatus::InProgress);
    }
    if (guardResult.transportStatus == TransportStatus::InProgress) {
      state_ = State::AwaitingTransmit;
      return setStatus(ObdServiceStatus::InProgress);
    }
    return finishTransportFailure();
  }

  ObdServiceStatus handleResponse(std::size_t responseLength) {
    if (responseLength == 0) {
      return setStatus(ObdServiceStatus::UnexpectedResponse);
    }

    const auto expectedService = expectedPositiveService();
    if (response_[0] != expectedService) {
      return setStatus(ObdServiceStatus::UnexpectedResponse);
    }
    if (requestKind_ != RequestKind::StoredDtcs &&
        (responseLength < 2 || response_[1] != requestedPid_)) {
      return setStatus(ObdServiceStatus::UnexpectedResponse);
    }

    switch (requestKind_) {
      case RequestKind::CurrentData:
        if (responseLength < 3) {
          return finish(ObdServiceStatus::InvalidResponse);
        }
        rawDataLength_ = responseLength - 2;
        if (rawDataLength_ > rawData_.size()) {
          return finish(ObdServiceStatus::InvalidResponse);
        }
        std::copy_n(response_.begin() + 2, rawDataLength_, rawData_.begin());
        return finish(ObdServiceStatus::ResponseReady);
      case RequestKind::SupportedPids:
        if (responseLength != 6) {
          return finish(ObdServiceStatus::InvalidResponse);
        }
        supportedPidBitmap_ =
            (static_cast<std::uint32_t>(response_[2]) << 24) |
            (static_cast<std::uint32_t>(response_[3]) << 16) |
            (static_cast<std::uint32_t>(response_[4]) << 8) |
            static_cast<std::uint32_t>(response_[5]);
        return finish(ObdServiceStatus::ResponseReady);
      case RequestKind::Vin:
        if (responseLength != 20 || response_[2] != 0x01) {
          return finish(ObdServiceStatus::InvalidResponse);
        }
        for (std::size_t index = 0; index < kVinLength; ++index) {
          vin_[index] = static_cast<char>(response_[index + 3]);
        }
        return finish(ObdServiceStatus::ResponseReady);
      case RequestKind::StoredDtcs: {
        const auto dtcPayloadLength = responseLength - 1;
        if ((dtcPayloadLength % 2) != 0) {
          return finish(ObdServiceStatus::InvalidResponse);
        }

        std::array<ObdDtcRecord, kMaxStoredDtcRecords> parsed{};
        std::size_t parsedCount = 0;
        for (std::size_t offset = 1; offset < responseLength; offset += 2) {
          const auto firstByte = response_[offset];
          const auto secondByte = response_[offset + 1];
          if (firstByte == 0 && secondByte == 0) {
            continue;
          }
          if (parsedCount == parsed.size()) {
            return finish(ObdServiceStatus::InvalidResponse);
          }
          parsed[parsedCount++] = {firstByte, secondByte};
        }

        storedDtcs_ = parsed;
        storedDtcCount_ = parsedCount;
        return finish(ObdServiceStatus::ResponseReady);
      }
      case RequestKind::None:
        return finish(ObdServiceStatus::InvalidResponse);
    }
    return finish(ObdServiceStatus::InvalidResponse);
  }

  void beginResponseWait() {
    state_ = State::AwaitingResponse;
    responseDeadlineMs_ = clock_.nowMs() + config_.responseTimeoutMs;
  }

  ObdServiceStatus finishTransportFailure() {
    return finish(ObdServiceStatus::TransportFailure);
  }

  ObdServiceStatus finishWithoutTransport(ObdServiceStatus status) {
    state_ = State::Idle;
    requestKind_ = RequestKind::None;
    return setStatus(status);
  }

  ObdServiceStatus finish(ObdServiceStatus status) {
    state_ = State::Idle;
    requestKind_ = RequestKind::None;
    return setStatus(status);
  }

  ObdServiceStatus setStatus(ObdServiceStatus status) {
    status_ = status;
    return status;
  }

  void clearResult() {
    rawDataLength_ = 0;
    supportedPidBitmap_ = 0;
    vin_.fill('\0');
    storedDtcs_.fill({});
    storedDtcCount_ = 0;
  }

  std::uint8_t expectedPositiveService() const {
    switch (requestKind_) {
      case RequestKind::Vin:
        return 0x49;
      case RequestKind::StoredDtcs:
        return 0x43;
      case RequestKind::CurrentData:
      case RequestKind::SupportedPids:
      case RequestKind::None:
        return 0x41;
    }
    return 0;
  }

  static bool isSupportedPidBlockBase(std::uint8_t pid) {
    return pid <= 0xC0 && (pid % 0x20) == 0;
  }

  static bool isNonFailingPollStatus(TransportStatus status) {
    return status == TransportStatus::Idle || status == TransportStatus::InProgress ||
           status == TransportStatus::Complete;
  }

  ReadOnlyGuard& guard_;
  Clock& clock_;
  ObdServiceConfig config_;
  State state_{State::Idle};
  RequestKind requestKind_{RequestKind::None};
  ObdServiceStatus status_{ObdServiceStatus::Idle};
  TransportStatus lastTransportStatus_{TransportStatus::Idle};
  std::uint8_t requestedPid_{0};
  std::uint64_t responseDeadlineMs_{0};
  std::array<std::uint8_t, kMaxRawDataLength> response_{};
  std::array<std::uint8_t, kMaxRawDataLength> rawData_{};
  std::size_t rawDataLength_{0};
  std::uint32_t supportedPidBitmap_{0};
  std::array<char, kVinLength> vin_{};
  std::array<ObdDtcRecord, kMaxStoredDtcRecords> storedDtcs_{};
  std::size_t storedDtcCount_{0};
};

}  // namespace vag_data
