#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include "../core/clock.h"
#include "read_only_guard.h"

namespace vag_data {

enum class UdsServiceStatus : std::uint8_t {
  Idle,
  InProgress,
  Busy,
  ResponseReady,
  ResponsePending,
  NegativeResponse,
  PendingLimitExceeded,
  Timeout,
  InvalidResponse,
  UnexpectedResponse,
  PolicyDenied,
  InvalidRequest,
  TransportFailure,
};

struct UdsServiceConfig {
  std::uint32_t responseTimeoutMs{1000};
  std::uint32_t responsePendingTimeoutMs{1000};
  std::uint8_t maxPendingResponses{3};
};

class UdsService {
 public:
  static constexpr std::size_t kMaxRawDataLength = 64;

  UdsService(ReadOnlyGuard& guard, Clock& clock,
             const UdsServiceConfig& config = {})
      : guard_(guard), clock_(clock), config_(config) {}

  UdsServiceStatus requestReadDataByIdentifier(std::uint16_t did) {
    if (isRequestActive()) {
      return setStatus(UdsServiceStatus::Busy);
    }
    if (config_.responseTimeoutMs == 0 ||
        config_.responsePendingTimeoutMs == 0) {
      return finishWithoutTransport(UdsServiceStatus::InvalidRequest);
    }

    clearResult();
    requestedDid_ = did;
    const auto guardResult = guard_.startUdsReadDataByIdentifier(did);
    lastTransportStatus_ = guardResult.transportStatus;
    if (guardResult.status == ReadOnlyStatus::PolicyDenied) {
      return finishWithoutTransport(UdsServiceStatus::PolicyDenied);
    }
    if (guardResult.status == ReadOnlyStatus::InvalidRequest) {
      return finishWithoutTransport(UdsServiceStatus::InvalidRequest);
    }
    if (guardResult.transportStatus == TransportStatus::Complete) {
      beginResponseWait(config_.responseTimeoutMs);
      return setStatus(UdsServiceStatus::InProgress);
    }
    if (guardResult.transportStatus == TransportStatus::InProgress) {
      state_ = State::AwaitingTransmit;
      return setStatus(UdsServiceStatus::InProgress);
    }
    return finishTransportFailure();
  }

  UdsServiceStatus poll() {
    if (state_ == State::Idle) {
      return UdsServiceStatus::Idle;
    }

    const auto pollStatus = guard_.poll();
    lastTransportStatus_ = pollStatus;
    if (state_ == State::AwaitingTransmit) {
      if (pollStatus == TransportStatus::InProgress) {
        return setStatus(UdsServiceStatus::InProgress);
      }
      if (pollStatus == TransportStatus::Complete) {
        beginResponseWait(config_.responseTimeoutMs);
        return setStatus(UdsServiceStatus::InProgress);
      }
      return finishTransportFailure();
    }

    if (!isNonFailingPollStatus(pollStatus)) {
      return finishTransportFailure();
    }
    if (clock_.nowMs() >= responseDeadlineMs_) {
      return finish(UdsServiceStatus::Timeout);
    }

    std::size_t responseLength = 0;
    const auto receiveStatus =
        guard_.receive(response_.data(), response_.size(), responseLength);
    lastTransportStatus_ = receiveStatus;
    if (receiveStatus == TransportStatus::NoData) {
      return setStatus(UdsServiceStatus::InProgress);
    }
    if (receiveStatus != TransportStatus::Complete) {
      return finishTransportFailure();
    }
    return handleResponse(responseLength);
  }

  UdsServiceStatus status() const { return status_; }
  bool isRequestActive() const { return state_ != State::Idle; }
  TransportStatus lastTransportStatus() const { return lastTransportStatus_; }
  std::uint8_t lastNrc() const { return lastNrc_; }
  std::size_t rawDataLength() const { return rawDataLength_; }
  const std::array<std::uint8_t, kMaxRawDataLength>& rawData() const {
    return rawData_;
  }

 private:
  enum class State : std::uint8_t {
    Idle,
    AwaitingTransmit,
    AwaitingResponse,
  };

  static constexpr std::uint8_t kReadDataByIdentifier = 0x22;
  static constexpr std::uint8_t kPositiveReadDataByIdentifier = 0x62;
  static constexpr std::uint8_t kNegativeResponse = 0x7F;
  static constexpr std::uint8_t kResponsePendingNrc = 0x78;

  UdsServiceStatus handleResponse(std::size_t responseLength) {
    if (responseLength == 0) {
      return setStatus(UdsServiceStatus::UnexpectedResponse);
    }

    if (response_[0] == kNegativeResponse) {
      return handleNegativeResponse(responseLength);
    }
    if (response_[0] != kPositiveReadDataByIdentifier) {
      return setStatus(UdsServiceStatus::UnexpectedResponse);
    }
    if (responseLength < 3) {
      return finish(UdsServiceStatus::InvalidResponse);
    }

    const auto responseDid = static_cast<std::uint16_t>(
        (static_cast<std::uint16_t>(response_[1]) << 8) | response_[2]);
    if (responseDid != requestedDid_) {
      return setStatus(UdsServiceStatus::UnexpectedResponse);
    }
    if (responseLength == 3) {
      return finish(UdsServiceStatus::InvalidResponse);
    }

    rawDataLength_ = responseLength - 3;
    if (rawDataLength_ > rawData_.size()) {
      return finish(UdsServiceStatus::InvalidResponse);
    }
    std::copy_n(response_.begin() + 3, rawDataLength_, rawData_.begin());
    return finish(UdsServiceStatus::ResponseReady);
  }

  UdsServiceStatus handleNegativeResponse(std::size_t responseLength) {
    if (responseLength != 3) {
      return finish(UdsServiceStatus::InvalidResponse);
    }
    if (response_[1] != kReadDataByIdentifier) {
      return setStatus(UdsServiceStatus::UnexpectedResponse);
    }

    lastNrc_ = response_[2];
    if (lastNrc_ != kResponsePendingNrc) {
      return finish(UdsServiceStatus::NegativeResponse);
    }
    if (pendingResponseCount_ >= config_.maxPendingResponses) {
      return finish(UdsServiceStatus::PendingLimitExceeded);
    }

    ++pendingResponseCount_;
    beginResponseWait(config_.responsePendingTimeoutMs);
    return setStatus(UdsServiceStatus::ResponsePending);
  }

  void beginResponseWait(std::uint32_t timeoutMs) {
    state_ = State::AwaitingResponse;
    responseDeadlineMs_ = clock_.nowMs() + timeoutMs;
  }

  UdsServiceStatus finishTransportFailure() {
    return finish(UdsServiceStatus::TransportFailure);
  }

  UdsServiceStatus finishWithoutTransport(UdsServiceStatus status) {
    state_ = State::Idle;
    return setStatus(status);
  }

  UdsServiceStatus finish(UdsServiceStatus status) {
    state_ = State::Idle;
    return setStatus(status);
  }

  UdsServiceStatus setStatus(UdsServiceStatus status) {
    status_ = status;
    return status;
  }

  void clearResult() {
    rawDataLength_ = 0;
    lastNrc_ = 0;
    pendingResponseCount_ = 0;
  }

  static bool isNonFailingPollStatus(TransportStatus status) {
    return status == TransportStatus::Idle || status == TransportStatus::InProgress ||
           status == TransportStatus::Complete;
  }

  ReadOnlyGuard& guard_;
  Clock& clock_;
  UdsServiceConfig config_;
  State state_{State::Idle};
  UdsServiceStatus status_{UdsServiceStatus::Idle};
  TransportStatus lastTransportStatus_{TransportStatus::Idle};
  std::uint16_t requestedDid_{0};
  std::uint8_t lastNrc_{0};
  std::uint8_t pendingResponseCount_{0};
  std::uint64_t responseDeadlineMs_{0};
  std::array<std::uint8_t, kMaxRawDataLength> response_{};
  std::array<std::uint8_t, kMaxRawDataLength> rawData_{};
  std::size_t rawDataLength_{0};
};

}  // namespace vag_data
