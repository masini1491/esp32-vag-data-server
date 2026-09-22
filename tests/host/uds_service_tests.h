#pragma once

#include <array>

#include "../../src/protocol/uds_service.h"
#include "fake_clock.h"
#include "read_only_guard_tests.h"
#include "test_helpers.h"

namespace vag_data::test {

inline void testUdsReadDataByIdentifierMatching() {
  RecordingDiagnosticTransport transport;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  UdsService service(guard, clock, {50, 10, 2});

  EXPECT_TRUE(service.requestReadDataByIdentifier(0xF190) ==
              UdsServiceStatus::InProgress);
  EXPECT_TRUE(transport.sendCount == 1);
  EXPECT_TRUE(transport.lastLength == 3);
  EXPECT_TRUE(transport.lastPayload[0] == 0x22);
  EXPECT_TRUE(transport.lastPayload[1] == 0xF1);
  EXPECT_TRUE(transport.lastPayload[2] == 0x90);

  transport.setResponse({0x63, 0xF1, 0x90, 0x01});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::UnexpectedResponse);
  EXPECT_TRUE(service.isRequestActive());

  transport.setResponse({0x62, 0xF1, 0x91, 0x01});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::UnexpectedResponse);
  EXPECT_TRUE(service.isRequestActive());

  transport.setResponse({0x62, 0xF1, 0x90});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::InvalidResponse);
  EXPECT_TRUE(!service.isRequestActive());

  EXPECT_TRUE(service.requestReadDataByIdentifier(0xF190) ==
              UdsServiceStatus::InProgress);
  transport.setResponse({0x62, 0xF1, 0x90, 0x12, 0x34});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponseReady);
  EXPECT_TRUE(service.rawDataLength() == 2);
  EXPECT_TRUE(service.rawData()[0] == 0x12 && service.rawData()[1] == 0x34);
}

inline void testUdsBusyTimeoutAndTransportFailures() {
  RecordingDiagnosticTransport transport;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  UdsService service(guard, clock, {50, 10, 2});

  EXPECT_TRUE(service.requestReadDataByIdentifier(0xF190) ==
              UdsServiceStatus::InProgress);
  EXPECT_TRUE(service.requestReadDataByIdentifier(0xF191) ==
              UdsServiceStatus::Busy);
  EXPECT_TRUE(transport.sendCount == 1);
  clock.advanceMs(50);
  EXPECT_TRUE(service.poll() == UdsServiceStatus::Timeout);

  constexpr std::array<TransportStatus, 5> startFailures{{
      TransportStatus::Busy,
      TransportStatus::TxBusy,
      TransportStatus::TxFailed,
      TransportStatus::BusOff,
      TransportStatus::NotInitialized,
  }};
  for (const auto failure : startFailures) {
    transport.sendStatus = failure;
    EXPECT_TRUE(service.requestReadDataByIdentifier(0xF190) ==
                UdsServiceStatus::TransportFailure);
    EXPECT_TRUE(service.lastTransportStatus() == failure);
  }

  transport.sendStatus = TransportStatus::Complete;
  EXPECT_TRUE(service.requestReadDataByIdentifier(0xF190) ==
              UdsServiceStatus::InProgress);
  transport.pollStatus = TransportStatus::BusOff;
  EXPECT_TRUE(service.poll() == UdsServiceStatus::TransportFailure);
  EXPECT_TRUE(service.lastTransportStatus() == TransportStatus::BusOff);

  transport.pollStatus = TransportStatus::Idle;
  EXPECT_TRUE(service.requestReadDataByIdentifier(0xF190) ==
              UdsServiceStatus::InProgress);
  transport.receiveStatus = TransportStatus::CanError;
  EXPECT_TRUE(service.poll() == UdsServiceStatus::TransportFailure);
  EXPECT_TRUE(service.lastTransportStatus() == TransportStatus::CanError);
}

inline void testUdsNegativeResponseSemantics() {
  RecordingDiagnosticTransport transport;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  UdsService service(guard, clock, {50, 10, 2});

  EXPECT_TRUE(service.requestReadDataByIdentifier(0xF190) ==
              UdsServiceStatus::InProgress);
  transport.setResponse({0x7F, 0x21, 0x31});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::UnexpectedResponse);
  EXPECT_TRUE(service.isRequestActive());

  transport.setResponse({0x7F, 0x22});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::InvalidResponse);
  EXPECT_TRUE(!service.isRequestActive());

  EXPECT_TRUE(service.requestReadDataByIdentifier(0xF190) ==
              UdsServiceStatus::InProgress);
  transport.setResponse({0x7F, 0x22, 0x31});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::NegativeResponse);
  EXPECT_TRUE(service.lastNrc() == 0x31);
  EXPECT_TRUE(!service.isRequestActive());
}

inline void testUdsWaitsForTransportCompletionBeforeTimeout() {
  RecordingDiagnosticTransport transport;
  transport.sendStatus = TransportStatus::InProgress;
  transport.pollStatus = TransportStatus::InProgress;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  UdsService service(guard, clock, {50, 10, 2});

  EXPECT_TRUE(service.requestReadDataByIdentifier(0xF190) ==
              UdsServiceStatus::InProgress);
  clock.advanceMs(100);
  EXPECT_TRUE(service.poll() == UdsServiceStatus::InProgress);
  transport.pollStatus = TransportStatus::Complete;
  EXPECT_TRUE(service.poll() == UdsServiceStatus::InProgress);
  clock.advanceMs(49);
  EXPECT_TRUE(service.poll() == UdsServiceStatus::InProgress);
  clock.advanceMs(1);
  EXPECT_TRUE(service.poll() == UdsServiceStatus::Timeout);
}

inline void testUdsResponsePendingIsBounded() {
  RecordingDiagnosticTransport transport;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  UdsService service(guard, clock, {50, 10, 2});

  EXPECT_TRUE(service.requestReadDataByIdentifier(0xF190) ==
              UdsServiceStatus::InProgress);
  transport.setResponse({0x7F, 0x22, 0x78});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponsePending);
  EXPECT_TRUE(service.isRequestActive());
  EXPECT_TRUE(service.lastNrc() == 0x78);

  clock.advanceMs(9);
  transport.setResponse({0x7F, 0x22, 0x78});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponsePending);
  clock.advanceMs(9);
  EXPECT_TRUE(service.poll() == UdsServiceStatus::InProgress);
  clock.advanceMs(1);
  EXPECT_TRUE(service.poll() == UdsServiceStatus::Timeout);

  EXPECT_TRUE(service.requestReadDataByIdentifier(0xF190) ==
              UdsServiceStatus::InProgress);
  transport.setResponse({0x7F, 0x22, 0x78});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponsePending);
  transport.setResponse({0x7F, 0x22, 0x78});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponsePending);
  transport.setResponse({0x7F, 0x22, 0x78});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::PendingLimitExceeded);
  EXPECT_TRUE(!service.isRequestActive());
}

inline void runUdsServiceTests() {
  testUdsReadDataByIdentifierMatching();
  testUdsBusyTimeoutAndTransportFailures();
  testUdsNegativeResponseSemantics();
  testUdsWaitsForTransportCompletionBeforeTimeout();
  testUdsResponsePendingIsBounded();
}

}  // namespace vag_data::test
