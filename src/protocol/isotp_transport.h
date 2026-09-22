#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include "../core/can_types.h"
#include "../core/clock.h"
#include "../hal/can_hal.h"
#include "diagnostic_transport.h"

namespace vag_data {

struct IsoTpConfig {
  CanId txId{0};
  CanId rxId{0};
  CanFrameFormat frameFormat{CanFrameFormat::Standard};
  std::size_t maxPayload{4095};
  std::uint32_t flowControlTimeoutMs{1000};
  std::uint32_t consecutiveFrameTimeoutMs{1000};
  std::uint8_t maxWaitFrames{3};
  std::uint8_t rxBlockSize{0};
  std::uint8_t rxStMin{0};
};

class IsoTpTransport final : public DiagnosticTransport {
 public:
  static constexpr std::size_t kMaxPayload = 4095;

  IsoTpTransport(CanHal& can, Clock& clock, const IsoTpConfig& config)
      : can_(can), clock_(clock), config_(config) {}

  TransportStatus startSend(const std::uint8_t* payload,
                            std::size_t length) override {
    if (!isConfigValid() || payload == nullptr || length == 0 ||
        length > config_.maxPayload || length > kMaxPayload) {
      return TransportStatus::InvalidArgument;
    }
    if (txState_ != TxState::Idle) {
      return TransportStatus::Busy;
    }

    if (length <= 7) {
      CanFrame frame = makeTxFrame();
      frame.length = static_cast<std::uint8_t>(length + 1);
      frame.payload[0] = static_cast<std::uint8_t>(length);
      std::copy_n(payload, length, frame.payload.begin() + 1);
      const auto status = sendFrame(frame);
      return status == TransportStatus::Complete ? TransportStatus::Complete
                                                 : status;
    }

    std::copy_n(payload, length, txPayload_.begin());
    txLength_ = length;
    txOffset_ = 6;
    txSequence_ = 1;
    txWaitCount_ = 0;

    CanFrame frame = makeTxFrame();
    frame.length = CanFrame::kClassicCanMaxPayload;
    frame.payload[0] =
        static_cast<std::uint8_t>(0x10U | ((length >> 8U) & 0x0FU));
    frame.payload[1] = static_cast<std::uint8_t>(length & 0xFFU);
    std::copy_n(payload, 6, frame.payload.begin() + 2);

    const auto status = sendFrame(frame);
    if (status != TransportStatus::Complete) {
      resetTx();
      return status;
    }
    txState_ = TxState::WaitFlowControl;
    txDeadlineMs_ = clock_.nowMs() + config_.flowControlTimeoutMs;
    return TransportStatus::InProgress;
  }

  TransportStatus poll() override {
    if (!isConfigValid()) {
      return TransportStatus::InvalidArgument;
    }

    const auto now = clock_.nowMs();
    if (rxState_ == RxState::Receiving && now >= rxDeadlineMs_) {
      resetRxAssembly();
      return TransportStatus::Timeout;
    }
    if (txState_ == TxState::WaitFlowControl && now >= txDeadlineMs_) {
      resetTx();
      return TransportStatus::Timeout;
    }
    if (txState_ == TxState::Sending && now >= txNextSendMs_) {
      return sendNextConsecutiveFrame(now);
    }
    return txState_ == TxState::Idle ? TransportStatus::Idle
                                     : TransportStatus::InProgress;
  }

  TransportStatus receive(std::uint8_t* payload, std::size_t capacity,
                          std::size_t& length) override {
    length = rxMessageLength_;
    if (!rxMessageReady_) {
      return TransportStatus::NoData;
    }
    if (payload == nullptr || capacity < rxMessageLength_) {
      return TransportStatus::Overflow;
    }
    std::copy_n(rxPayload_.begin(), rxMessageLength_, payload);
    rxMessageReady_ = false;
    rxMessageLength_ = 0;
    return TransportStatus::Complete;
  }

  // The CAN owner/dispatcher supplies frames. This class never drains
  // CanHal::receive(), so unrelated traffic is not silently consumed here.
  TransportStatus onCanFrame(const CanFrame& frame) {
    if (!isConfigValid()) {
      return TransportStatus::InvalidArgument;
    }
    if (frame.id != config_.rxId || frame.format != config_.frameFormat) {
      return TransportStatus::Ignored;
    }
    if (!frame.isValid() || frame.length == 0) {
      return TransportStatus::InvalidFrame;
    }

    const auto now = clock_.nowMs();
    if (rxState_ == RxState::Receiving && now >= rxDeadlineMs_) {
      resetRxAssembly();
      return TransportStatus::Timeout;
    }
    if (txState_ == TxState::WaitFlowControl && now >= txDeadlineMs_) {
      resetTx();
      return TransportStatus::Timeout;
    }

    switch ((frame.payload[0] >> 4U) & 0x0FU) {
      case 0x0:
        return handleSingleFrame(frame);
      case 0x1:
        return handleFirstFrame(frame, now);
      case 0x2:
        return handleConsecutiveFrame(frame, now);
      case 0x3:
        return handleFlowControl(frame, now);
      default:
        return TransportStatus::InvalidFrame;
    }
  }

 private:
  enum class RxState : std::uint8_t { Idle, Receiving };
  enum class TxState : std::uint8_t { Idle, WaitFlowControl, Sending };

  bool isConfigValid() const {
    CanFrame tx = makeTxFrame();
    CanFrame rx;
    rx.id = config_.rxId;
    rx.format = config_.frameFormat;
    return tx.isValid() && rx.isValid() && config_.maxPayload > 0 &&
           config_.maxPayload <= kMaxPayload &&
           config_.flowControlTimeoutMs > 0 &&
           config_.consecutiveFrameTimeoutMs > 0 &&
           isStMinCodeValid(config_.rxStMin);
  }

  static bool isStMinCodeValid(std::uint8_t value) {
    return value <= 0x7F || (value >= 0xF1 && value <= 0xF9);
  }

  // Clock has millisecond resolution. ISO-TP 100-900 us values are therefore
  // conservatively rounded up to 1 ms instead of being treated as 0 ms.
  static bool decodeStMinMs(std::uint8_t value, std::uint32_t& milliseconds) {
    if (value <= 0x7F) {
      milliseconds = value;
      return true;
    }
    if (value >= 0xF1 && value <= 0xF9) {
      milliseconds = 1;
      return true;
    }
    return false;
  }

  CanFrame makeTxFrame() const {
    CanFrame frame;
    frame.id = config_.txId;
    frame.format = config_.frameFormat;
    return frame;
  }

  static TransportStatus mapCanStatus(CanStatus status) {
    switch (status) {
      case CanStatus::Ok:
        return TransportStatus::Complete;
      case CanStatus::TxBusy:
        return TransportStatus::TxBusy;
      case CanStatus::TxFailed:
        return TransportStatus::TxFailed;
      case CanStatus::BusOff:
        return TransportStatus::BusOff;
      case CanStatus::NotInitialized:
        return TransportStatus::NotInitialized;
      case CanStatus::InvalidConfig:
        return TransportStatus::InvalidArgument;
      case CanStatus::NoData:
      case CanStatus::DriverError:
      case CanStatus::RxOverflow:
        return TransportStatus::CanError;
    }
    return TransportStatus::CanError;
  }

  TransportStatus sendFrame(const CanFrame& frame) {
    return mapCanStatus(can_.send(frame));
  }

  TransportStatus handleSingleFrame(const CanFrame& frame) {
    if (rxState_ != RxState::Idle || rxMessageReady_) {
      return TransportStatus::Busy;
    }
    const std::size_t length = frame.payload[0] & 0x0FU;
    if (length == 0 || length > 7 || frame.length < length + 1 ||
        length > config_.maxPayload) {
      return TransportStatus::InvalidFrame;
    }
    std::copy_n(frame.payload.begin() + 1, length, rxPayload_.begin());
    rxMessageLength_ = length;
    rxMessageReady_ = true;
    return TransportStatus::MessageReady;
  }

  TransportStatus handleFirstFrame(const CanFrame& frame, std::uint64_t now) {
    if (rxState_ != RxState::Idle || rxMessageReady_) {
      return TransportStatus::Busy;
    }
    if (frame.length != CanFrame::kClassicCanMaxPayload) {
      return TransportStatus::InvalidFrame;
    }
    const std::size_t declaredLength =
        (static_cast<std::size_t>(frame.payload[0] & 0x0FU) << 8U) |
        frame.payload[1];
    if (declaredLength <= 7 || declaredLength > kMaxPayload) {
      return TransportStatus::InvalidFrame;
    }
    if (declaredLength > config_.maxPayload) {
      const auto status = sendFlowControl(0x2);
      return status == TransportStatus::Complete ? TransportStatus::Overflow
                                                 : status;
    }

    rxExpectedLength_ = declaredLength;
    rxOffset_ = std::min<std::size_t>(frame.length - 2, declaredLength);
    std::copy_n(frame.payload.begin() + 2, rxOffset_, rxPayload_.begin());
    rxExpectedSequence_ = 1;
    rxBlockCount_ = 0;
    rxState_ = RxState::Receiving;
    rxDeadlineMs_ = now + config_.consecutiveFrameTimeoutMs;

    const auto status = sendFlowControl(0x0);
    if (status != TransportStatus::Complete) {
      resetRxAssembly();
      return status;
    }
    return TransportStatus::InProgress;
  }

  TransportStatus handleConsecutiveFrame(const CanFrame& frame,
                                         std::uint64_t now) {
    if (rxState_ != RxState::Receiving) {
      return TransportStatus::UnexpectedFrame;
    }
    if (frame.length < 2) {
      resetRxAssembly();
      return TransportStatus::InvalidFrame;
    }
    const auto sequence = frame.payload[0] & 0x0FU;
    if (sequence != rxExpectedSequence_) {
      resetRxAssembly();
      return TransportStatus::SequenceError;
    }

    const auto remaining = rxExpectedLength_ - rxOffset_;
    const auto copyLength = std::min<std::size_t>(frame.length - 1, remaining);
    std::copy_n(frame.payload.begin() + 1, copyLength,
                rxPayload_.begin() + rxOffset_);
    rxOffset_ += copyLength;
    rxExpectedSequence_ = static_cast<std::uint8_t>((sequence + 1U) & 0x0FU);
    ++rxBlockCount_;

    if (rxOffset_ == rxExpectedLength_) {
      rxMessageLength_ = rxExpectedLength_;
      rxMessageReady_ = true;
      resetRxAssembly();
      return TransportStatus::MessageReady;
    }

    rxDeadlineMs_ = now + config_.consecutiveFrameTimeoutMs;
    if (config_.rxBlockSize != 0 && rxBlockCount_ >= config_.rxBlockSize) {
      rxBlockCount_ = 0;
      const auto status = sendFlowControl(0x0);
      if (status != TransportStatus::Complete) {
        resetRxAssembly();
        return status;
      }
    }
    return TransportStatus::InProgress;
  }

  TransportStatus handleFlowControl(const CanFrame& frame,
                                    std::uint64_t now) {
    if (txState_ != TxState::WaitFlowControl) {
      return TransportStatus::UnexpectedFrame;
    }
    if (frame.length < 3) {
      resetTx();
      return TransportStatus::InvalidFrame;
    }

    const auto flowStatus = frame.payload[0] & 0x0FU;
    if (flowStatus == 0x0) {
      std::uint32_t separationMs = 0;
      if (!decodeStMinMs(frame.payload[2], separationMs)) {
        resetTx();
        return TransportStatus::InvalidFrame;
      }
      txBlockSize_ = frame.payload[1];
      txBlockRemaining_ = txBlockSize_;
      txSeparationMs_ = separationMs;
      txNextSendMs_ = now;
      txState_ = TxState::Sending;
      return TransportStatus::InProgress;
    }
    if (flowStatus == 0x1) {
      ++txWaitCount_;
      if (txWaitCount_ > config_.maxWaitFrames) {
        resetTx();
        return TransportStatus::WaitLimitExceeded;
      }
      txDeadlineMs_ = now + config_.flowControlTimeoutMs;
      return TransportStatus::InProgress;
    }
    if (flowStatus == 0x2) {
      resetTx();
      return TransportStatus::FlowControlOverflow;
    }
    resetTx();
    return TransportStatus::InvalidFrame;
  }

  TransportStatus sendFlowControl(std::uint8_t flowStatus) {
    CanFrame frame = makeTxFrame();
    frame.length = 3;
    frame.payload[0] = static_cast<std::uint8_t>(0x30U | flowStatus);
    frame.payload[1] = config_.rxBlockSize;
    frame.payload[2] = config_.rxStMin;
    return sendFrame(frame);
  }

  TransportStatus sendNextConsecutiveFrame(std::uint64_t now) {
    CanFrame frame = makeTxFrame();
    const auto remaining = txLength_ - txOffset_;
    const auto copyLength = std::min<std::size_t>(7, remaining);
    frame.length = static_cast<std::uint8_t>(copyLength + 1);
    frame.payload[0] = static_cast<std::uint8_t>(0x20U | txSequence_);
    std::copy_n(txPayload_.begin() + txOffset_, copyLength,
                frame.payload.begin() + 1);

    const auto status = sendFrame(frame);
    if (status == TransportStatus::TxBusy) {
      return status;
    }
    if (status != TransportStatus::Complete) {
      resetTx();
      return status;
    }

    txOffset_ += copyLength;
    txSequence_ = static_cast<std::uint8_t>((txSequence_ + 1U) & 0x0FU);
    if (txOffset_ == txLength_) {
      resetTx();
      return TransportStatus::Complete;
    }

    if (txBlockSize_ != 0) {
      --txBlockRemaining_;
      if (txBlockRemaining_ == 0) {
        txState_ = TxState::WaitFlowControl;
        txDeadlineMs_ = now + config_.flowControlTimeoutMs;
        return TransportStatus::InProgress;
      }
    }
    txNextSendMs_ = now + txSeparationMs_;
    return TransportStatus::InProgress;
  }

  void resetRxAssembly() {
    rxState_ = RxState::Idle;
    rxExpectedLength_ = 0;
    rxOffset_ = 0;
    rxExpectedSequence_ = 1;
    rxBlockCount_ = 0;
    rxDeadlineMs_ = 0;
  }

  void resetTx() {
    txState_ = TxState::Idle;
    txLength_ = 0;
    txOffset_ = 0;
    txSequence_ = 1;
    txBlockSize_ = 0;
    txBlockRemaining_ = 0;
    txWaitCount_ = 0;
    txSeparationMs_ = 0;
    txDeadlineMs_ = 0;
    txNextSendMs_ = 0;
  }

  CanHal& can_;
  Clock& clock_;
  IsoTpConfig config_;
  std::array<std::uint8_t, kMaxPayload> rxPayload_{};
  std::array<std::uint8_t, kMaxPayload> txPayload_{};
  RxState rxState_{RxState::Idle};
  TxState txState_{TxState::Idle};
  std::size_t rxExpectedLength_{0};
  std::size_t rxOffset_{0};
  std::size_t rxMessageLength_{0};
  std::size_t txLength_{0};
  std::size_t txOffset_{0};
  std::uint8_t rxExpectedSequence_{1};
  std::uint8_t rxBlockCount_{0};
  std::uint8_t txSequence_{1};
  std::uint8_t txBlockSize_{0};
  std::uint8_t txBlockRemaining_{0};
  std::uint8_t txWaitCount_{0};
  std::uint32_t txSeparationMs_{0};
  std::uint64_t rxDeadlineMs_{0};
  std::uint64_t txDeadlineMs_{0};
  std::uint64_t txNextSendMs_{0};
  bool rxMessageReady_{false};
};

}  // namespace vag_data
