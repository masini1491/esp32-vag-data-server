#pragma once

#include <cstddef>
#include <cstdint>

namespace vag_data {

enum class TransportStatus : std::uint8_t {
  Idle,
  InProgress,
  Complete,
  MessageReady,
  NoData,
  Ignored,
  Busy,
  InvalidArgument,
  InvalidFrame,
  UnexpectedFrame,
  Overflow,
  SequenceError,
  Timeout,
  WaitLimitExceeded,
  FlowControlOverflow,
  TxBusy,
  TxFailed,
  BusOff,
  NotInitialized,
  CanError,
};

// Internal diagnostic-service boundary. When integrated with application code,
// callers must remain behind the repository's diagnostic policy gate.
// Complete means all transport frames were accepted by CanHal; it does not
// prove on-wire transmission, ACK, or peer reception.
class DiagnosticTransport {
 public:
  virtual ~DiagnosticTransport() = default;

  virtual TransportStatus startSend(const std::uint8_t* payload,
                                    std::size_t length) = 0;
  virtual TransportStatus poll() = 0;
  virtual TransportStatus receive(std::uint8_t* payload,
                                  std::size_t capacity,
                                  std::size_t& length) = 0;
};

}  // namespace vag_data
