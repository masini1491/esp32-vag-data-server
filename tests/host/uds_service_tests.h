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

inline void testUdsDtcMatchingAndRawRecords() {
  RecordingDiagnosticTransport transport;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  UdsService service(guard, clock, {50, 10, 2});

  EXPECT_TRUE(service.requestReportDtcByStatusMask(0xA5) ==
              UdsServiceStatus::InProgress);
  EXPECT_TRUE(transport.lastLength == 3);
  EXPECT_TRUE(transport.lastPayload[0] == 0x19);
  EXPECT_TRUE(transport.lastPayload[1] == 0x02);
  EXPECT_TRUE(transport.lastPayload[2] == 0xA5);
  EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::Busy);
  EXPECT_TRUE(service.requestReadDataByIdentifier(0xF190) == UdsServiceStatus::Busy);
  EXPECT_TRUE(transport.sendCount == 1);

  transport.setResponse({0x62, 0xF1, 0x90, 0x01});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::UnexpectedResponse);
  transport.setResponse({0x59, 0x01, 0xFF});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::UnexpectedResponse);
  EXPECT_TRUE(service.isRequestActive());
  EXPECT_TRUE(service.dtcRecordCount() == 0);
  EXPECT_TRUE(service.dtcStatusAvailabilityMask() == 0);

  transport.setResponse({0x59, 0x02, 0x81});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponseReady);
  EXPECT_TRUE(service.dtcRecordCount() == 0);
  EXPECT_TRUE(service.dtcStatusAvailabilityMask() == 0x81);

  EXPECT_TRUE(service.requestReportDtcByStatusMask(0x01) ==
              UdsServiceStatus::InProgress);
  EXPECT_TRUE(service.dtcStatusAvailabilityMask() == 0);
  transport.setResponse({0x59, 0x02, 0xFF, 0x12, 0x34, 0x56, 0xA5});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponseReady);
  EXPECT_TRUE(service.dtcRecordCount() == 1);
  EXPECT_TRUE(service.dtcRecords()[0].dtc[0] == 0x12);
  EXPECT_TRUE(service.dtcRecords()[0].dtc[1] == 0x34);
  EXPECT_TRUE(service.dtcRecords()[0].dtc[2] == 0x56);
  EXPECT_TRUE(service.dtcRecords()[0].status == 0xA5);
  EXPECT_TRUE(service.rawDataLength() == 0);

  EXPECT_TRUE(service.requestReportDtcByStatusMask(0x00) ==
              UdsServiceStatus::InProgress);
  transport.setResponse({0x59, 0x02, 0x7F, 0x00, 0x00, 0x00, 0x00,
                         0xAB, 0xCD, 0xEF, 0x82});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponseReady);
  EXPECT_TRUE(service.dtcRecordCount() == 2);  // Zero DTC is a raw record, not padding.
  EXPECT_TRUE(service.dtcRecords()[0].dtc == (std::array<std::uint8_t, 3>{0, 0, 0}));
  EXPECT_TRUE(service.dtcRecords()[0].status == 0);
  EXPECT_TRUE(service.dtcRecords()[1].dtc ==
              (std::array<std::uint8_t, 3>{0xAB, 0xCD, 0xEF}));
  EXPECT_TRUE(service.dtcRecords()[1].status == 0x82);
  EXPECT_TRUE(service.dtcStatusAvailabilityMask() == 0x7F);

  EXPECT_TRUE(service.requestReadDataByIdentifier(0xF190) ==
              UdsServiceStatus::InProgress);
  EXPECT_TRUE(service.dtcRecordCount() == 0);
  EXPECT_TRUE(service.dtcRecords()[1].status == 0);
  EXPECT_TRUE(service.requestReportDtcByStatusMask(0x01) == UdsServiceStatus::Busy);
  transport.setResponse({0x59, 0x02, 0xFF});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::UnexpectedResponse);
  transport.setResponse({0x62, 0xF1, 0x90, 0x42});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponseReady);
  EXPECT_TRUE(service.rawDataLength() == 1 && service.rawData()[0] == 0x42);
}

inline void testUdsDtcMalformedAndCapacity() {
  RecordingDiagnosticTransport transport;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  UdsService service(guard, clock);

  // Seed a successful result, then prove failure paths publish no stale/partial result.
  EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InProgress);
  transport.setResponse({0x59, 0x02, 0xFF, 0x12, 0x34, 0x56, 0x01});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponseReady);
  for (const auto bytes : {std::initializer_list<std::uint8_t>{},
                           {0x59}, {0x59, 0x02}, {0x59, 0x02, 0xA5, 0x12},
                           {0x59, 0x02, 0xA5, 0x12, 0x34},
                           {0x59, 0x02, 0xA5, 0x12, 0x34, 0x56}}) {
    EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InProgress);
    transport.setResponse(bytes);
    EXPECT_TRUE(service.poll() == UdsServiceStatus::InvalidResponse);
    EXPECT_TRUE(!service.isRequestActive());
    EXPECT_TRUE(service.dtcRecordCount() == 0);
    EXPECT_TRUE(service.dtcStatusAvailabilityMask() == 0);
    EXPECT_TRUE(service.dtcRecords()[0].status == 0);
  }

  static_assert(UdsService::kMaxDtcRecords == 15);
  std::array<std::uint8_t, 3 + 4 * UdsService::kMaxDtcRecords> maximum{};
  maximum[0] = 0x59;
  maximum[1] = 0x02;
  maximum[2] = 0xFF;
  maximum.back() = 0x80;
  EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InProgress);
  transport.setResponse(maximum.data(), maximum.size());
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponseReady);
  EXPECT_TRUE(service.dtcRecordCount() == UdsService::kMaxDtcRecords);
  EXPECT_TRUE(service.dtcRecords().back().status == 0x80);

  std::array<std::uint8_t, 3 + 4 * (UdsService::kMaxDtcRecords + 1)> overflow{};
  overflow[0] = 0x59;
  overflow[1] = 0x02;
  overflow[2] = 0xFF;
  EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InProgress);
  transport.setResponse(overflow.data(), overflow.size());
  EXPECT_TRUE(service.poll() == UdsServiceStatus::TransportFailure);
  EXPECT_TRUE(service.lastTransportStatus() == TransportStatus::Overflow);
  EXPECT_TRUE(!service.isRequestActive());
  EXPECT_TRUE(service.dtcRecordCount() == 0);
  EXPECT_TRUE(service.dtcStatusAvailabilityMask() == 0);
}

inline void testUdsDtcNegativeAndBoundedPending() {
  RecordingDiagnosticTransport transport;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  UdsService service(guard, clock, {50, 10, 2});
  EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InProgress);
  transport.setResponse({0x7F, 0x22, 0x78});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::UnexpectedResponse);
  EXPECT_TRUE(service.lastNrc() == 0);
  EXPECT_TRUE(service.isRequestActive());
  transport.setResponse({0x7F, 0x19, 0x31});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::NegativeResponse);
  EXPECT_TRUE(service.lastNrc() == 0x31);
  EXPECT_TRUE(!service.isRequestActive());

  for (const auto bytes : {std::initializer_list<std::uint8_t>{0x7F, 0x19},
                           {0x7F, 0x19, 0x31, 0x00}}) {
    EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InProgress);
    transport.setResponse(bytes);
    EXPECT_TRUE(service.poll() == UdsServiceStatus::InvalidResponse);
    EXPECT_TRUE(!service.isRequestActive());
  }

  EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InProgress);
  transport.setResponse({0x7F, 0x19, 0x78});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponsePending);
  clock.advanceMs(9);
  transport.setResponse({0x7F, 0x19, 0x78});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponsePending);
  transport.setResponse({0x7F, 0x19, 0x78});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::PendingLimitExceeded);
  EXPECT_TRUE(!service.isRequestActive());

  EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InProgress);
  transport.setResponse({0x7F, 0x19, 0x78});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponsePending);
  clock.advanceMs(9);
  EXPECT_TRUE(service.poll() == UdsServiceStatus::InProgress);
  clock.advanceMs(1);
  EXPECT_TRUE(service.poll() == UdsServiceStatus::Timeout);

  EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InProgress);
  transport.setResponse({0x7F, 0x19, 0x78});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponsePending);
  transport.setResponse({0x59, 0x02, 0xFF});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponseReady);
  EXPECT_TRUE(service.dtcStatusAvailabilityMask() == 0xFF);
}

inline void testUdsDtcTimeoutAndTransportFailures() {
  RecordingDiagnosticTransport transport;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  UdsService service(guard, clock, {50, 10, 2});
  for (const auto failure : {TransportStatus::Busy, TransportStatus::TxBusy,
                             TransportStatus::TxFailed, TransportStatus::BusOff,
                             TransportStatus::NotInitialized}) {
    transport.sendStatus = failure;
    EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::TransportFailure);
    EXPECT_TRUE(service.lastTransportStatus() == failure);
    EXPECT_TRUE(!service.isRequestActive());
  }
  transport.sendStatus = TransportStatus::Complete;
  EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InProgress);
  clock.advanceMs(50);
  EXPECT_TRUE(service.poll() == UdsServiceStatus::Timeout);

  transport.sendStatus = TransportStatus::InProgress;
  transport.pollStatus = TransportStatus::InProgress;
  EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InProgress);
  clock.advanceMs(100);
  EXPECT_TRUE(service.poll() == UdsServiceStatus::InProgress);
  transport.pollStatus = TransportStatus::Complete;
  EXPECT_TRUE(service.poll() == UdsServiceStatus::InProgress);
  transport.setResponse({0x59, 0x02, 0xFF});
  EXPECT_TRUE(service.poll() == UdsServiceStatus::ResponseReady);

  transport.sendStatus = TransportStatus::Complete;
  EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InProgress);
  transport.pollStatus = TransportStatus::BusOff;
  EXPECT_TRUE(service.poll() == UdsServiceStatus::TransportFailure);
  EXPECT_TRUE(service.lastTransportStatus() == TransportStatus::BusOff);
  transport.pollStatus = TransportStatus::Idle;
  EXPECT_TRUE(service.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InProgress);
  transport.receiveStatus = TransportStatus::CanError;
  EXPECT_TRUE(service.poll() == UdsServiceStatus::TransportFailure);
  EXPECT_TRUE(service.lastTransportStatus() == TransportStatus::CanError);

  UdsService invalidConfig(guard, clock, {0, 10, 2});
  const auto previousSendCount = transport.sendCount;
  EXPECT_TRUE(invalidConfig.requestReportDtcByStatusMask(0xFF) == UdsServiceStatus::InvalidRequest);
  EXPECT_TRUE(transport.sendCount == previousSendCount);
}

inline void runUdsServiceTests() {
  testUdsReadDataByIdentifierMatching();
  testUdsBusyTimeoutAndTransportFailures();
  testUdsNegativeResponseSemantics();
  testUdsWaitsForTransportCompletionBeforeTimeout();
  testUdsResponsePendingIsBounded();
  testUdsDtcMatchingAndRawRecords();
  testUdsDtcMalformedAndCapacity();
  testUdsDtcNegativeAndBoundedPending();
  testUdsDtcTimeoutAndTransportFailures();
}

}  // namespace vag_data::test
