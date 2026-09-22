#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>

#include "../../src/protocol/read_only_guard.h"
#include "test_helpers.h"

namespace vag_data::test {

class RecordingDiagnosticTransport final : public DiagnosticTransport {
 public:
  TransportStatus startSend(const std::uint8_t* payload,
                            std::size_t length) override {
    ++sendCount;
    lastLength = length;
    for (std::size_t index = 0; index < length && index < lastPayload.size();
         ++index) {
      lastPayload[index] = payload[index];
    }
    return sendStatus;
  }

  TransportStatus poll() override { return pollStatus; }

  TransportStatus receive(std::uint8_t* payload, std::size_t capacity,
                          std::size_t& length) override {
    const auto status = receiveStatus;
    length = responseLength;
    if (status == TransportStatus::Complete) {
      if (payload == nullptr || capacity < responseLength) {
        return TransportStatus::Overflow;
      }
      for (std::size_t index = 0; index < responseLength; ++index) {
        payload[index] = response[index];
      }
      receiveStatus = TransportStatus::NoData;
    }
    return status;
  }

  void setResponse(std::initializer_list<std::uint8_t> bytes) {
    responseLength = bytes.size();
    std::size_t index = 0;
    for (const auto value : bytes) {
      response[index++] = value;
    }
    receiveStatus = TransportStatus::Complete;
  }

  TransportStatus sendStatus = TransportStatus::Complete;
  TransportStatus pollStatus = TransportStatus::Idle;
  TransportStatus receiveStatus = TransportStatus::NoData;
  std::array<std::uint8_t, 2> lastPayload{};
  std::array<std::uint8_t, 64> response{};
  std::size_t lastLength = 0;
  std::size_t sendCount = 0;
  std::size_t responseLength = 0;
};

inline void testReadOnlyGuardAllowedRequests() {
  RecordingDiagnosticTransport transport;
  ReadOnlyGuard guard(transport);
  const std::uint8_t mode01Pid = 0x0C;
  const auto mode01 = guard.startObdSinglePid({0x01, &mode01Pid, 1});
  EXPECT_TRUE(mode01.status == ReadOnlyStatus::Forwarded);
  EXPECT_TRUE(mode01.transportStatus == TransportStatus::Complete);
  EXPECT_TRUE(transport.sendCount == 1);
  EXPECT_TRUE(transport.lastLength == 2);
  EXPECT_TRUE(transport.lastPayload[0] == 0x01);
  EXPECT_TRUE(transport.lastPayload[1] == 0x0C);

  const std::uint8_t mode09Pid = 0x02;
  const auto mode09 = guard.startObdSinglePid({0x09, &mode09Pid, 1});
  EXPECT_TRUE(mode09.status == ReadOnlyStatus::Forwarded);
  EXPECT_TRUE(mode09.transportStatus == TransportStatus::Complete);
  EXPECT_TRUE(transport.sendCount == 2);
  EXPECT_TRUE(transport.lastLength == 2);
  EXPECT_TRUE(transport.lastPayload[0] == 0x09);
  EXPECT_TRUE(transport.lastPayload[1] == 0x02);
}

inline void testReadOnlyGuardDeniesAndRejectsMalformedRequests() {
  RecordingDiagnosticTransport transport;
  ReadOnlyGuard guard(transport);
  const std::uint8_t pid = 0x00;

  const auto mode04 = guard.startObdSinglePid({0x04, &pid, 1});
  EXPECT_TRUE(mode04.status == ReadOnlyStatus::PolicyDenied);
  EXPECT_TRUE(transport.sendCount == 0);

  const auto mode03 = guard.startObdSinglePid({0x03, &pid, 1});
  EXPECT_TRUE(mode03.status == ReadOnlyStatus::PolicyDenied);
  EXPECT_TRUE(transport.sendCount == 0);

  const auto nullPid = guard.startObdSinglePid({0x01, nullptr, 1});
  EXPECT_TRUE(nullPid.status == ReadOnlyStatus::InvalidRequest);
  EXPECT_TRUE(transport.sendCount == 0);

  const auto missingPid = guard.startObdSinglePid({0x01, &pid, 0});
  EXPECT_TRUE(missingPid.status == ReadOnlyStatus::InvalidRequest);
  EXPECT_TRUE(transport.sendCount == 0);

  const auto multiplePids = guard.startObdSinglePid({0x01, &pid, 2});
  EXPECT_TRUE(multiplePids.status == ReadOnlyStatus::InvalidRequest);
  EXPECT_TRUE(transport.sendCount == 0);
}

inline void testReadOnlyGuardPreservesTransportStatus() {
  constexpr std::array<TransportStatus, 5> statuses{{
      TransportStatus::Busy,
      TransportStatus::TxBusy,
      TransportStatus::TxFailed,
      TransportStatus::BusOff,
      TransportStatus::NotInitialized,
  }};

  for (const auto status : statuses) {
    RecordingDiagnosticTransport transport;
    transport.sendStatus = status;
    ReadOnlyGuard guard(transport);
    const std::uint8_t pid = 0x05;
    const auto result = guard.startObdSinglePid({0x01, &pid, 1});
    EXPECT_TRUE(result.status == ReadOnlyStatus::Forwarded);
    EXPECT_TRUE(result.transportStatus == status);
    EXPECT_TRUE(transport.sendCount == 1);
  }
}

inline void testReadOnlyGuardDelegatesPollAndReceive() {
  RecordingDiagnosticTransport transport;
  transport.pollStatus = TransportStatus::Busy;
  transport.receiveStatus = TransportStatus::NoData;
  transport.responseLength = 0;
  ReadOnlyGuard guard(transport);
  std::array<std::uint8_t, 8> payload{};
  std::size_t length = 99;

  EXPECT_TRUE(guard.poll() == TransportStatus::Busy);
  EXPECT_TRUE(guard.receive(payload.data(), payload.size(), length) ==
              TransportStatus::NoData);
  EXPECT_TRUE(length == 0);
}

inline void runReadOnlyGuardTests() {
  testReadOnlyGuardAllowedRequests();
  testReadOnlyGuardDeniesAndRejectsMalformedRequests();
  testReadOnlyGuardPreservesTransportStatus();
  testReadOnlyGuardDelegatesPollAndReceive();
}

}  // namespace vag_data::test
