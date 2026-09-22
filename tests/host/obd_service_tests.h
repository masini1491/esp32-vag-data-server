#pragma once

#include <array>
#include <cstdint>

#include "../../src/protocol/obd_service.h"
#include "fake_clock.h"
#include "read_only_guard_tests.h"
#include "test_helpers.h"

namespace vag_data::test {

inline void testObdCurrentDataAndResponseMatching() {
  RecordingDiagnosticTransport transport;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  ObdService service(guard, clock, {50});

  EXPECT_TRUE(service.requestCurrentData(0x0C) == ObdServiceStatus::InProgress);
  EXPECT_TRUE(transport.sendCount == 1);
  EXPECT_TRUE(transport.lastPayload[0] == 0x01 && transport.lastPayload[1] == 0x0C);

  transport.setResponse({0x42, 0x0C, 0x00});
  EXPECT_TRUE(service.poll() == ObdServiceStatus::UnexpectedResponse);
  EXPECT_TRUE(service.isRequestActive());

  transport.setResponse({0x41, 0x0D, 0x00});
  EXPECT_TRUE(service.poll() == ObdServiceStatus::UnexpectedResponse);
  EXPECT_TRUE(service.isRequestActive());

  transport.setResponse({0x41, 0x0C});
  const auto malformedCurrentDataStatus = service.poll();
  EXPECT_TRUE(malformedCurrentDataStatus == ObdServiceStatus::InvalidResponse);
  EXPECT_TRUE(!service.isRequestActive());

  EXPECT_TRUE(service.requestCurrentData(0x0C) == ObdServiceStatus::InProgress);
  transport.setResponse({0x41, 0x0C, 0x1A, 0xF8});
  EXPECT_TRUE(service.poll() == ObdServiceStatus::ResponseReady);
  EXPECT_TRUE(service.rawDataLength() == 2);
  EXPECT_TRUE(service.rawData()[0] == 0x1A && service.rawData()[1] == 0xF8);
}

inline void testObdBusyTimeoutAndTransportFailures() {
  RecordingDiagnosticTransport transport;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  ObdService service(guard, clock, {50});

  EXPECT_TRUE(service.requestCurrentData(0x05) == ObdServiceStatus::InProgress);
  EXPECT_TRUE(service.requestCurrentData(0x0C) == ObdServiceStatus::Busy);
  EXPECT_TRUE(transport.sendCount == 1);
  clock.advanceMs(50);
  EXPECT_TRUE(service.poll() == ObdServiceStatus::Timeout);

  constexpr std::array<TransportStatus, 5> startFailures{{
      TransportStatus::Busy,
      TransportStatus::TxBusy,
      TransportStatus::TxFailed,
      TransportStatus::BusOff,
      TransportStatus::NotInitialized,
  }};
  for (const auto failure : startFailures) {
    transport.sendStatus = failure;
    EXPECT_TRUE(service.requestCurrentData(0x05) ==
                ObdServiceStatus::TransportFailure);
    EXPECT_TRUE(service.lastTransportStatus() == failure);
  }

  transport.sendStatus = TransportStatus::Complete;
  EXPECT_TRUE(service.requestCurrentData(0x05) == ObdServiceStatus::InProgress);
  transport.pollStatus = TransportStatus::BusOff;
  EXPECT_TRUE(service.poll() == ObdServiceStatus::TransportFailure);
  EXPECT_TRUE(service.lastTransportStatus() == TransportStatus::BusOff);
}

inline void testObdWaitsForTransportCompletionBeforeTimeout() {
  RecordingDiagnosticTransport transport;
  transport.sendStatus = TransportStatus::InProgress;
  transport.pollStatus = TransportStatus::InProgress;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  ObdService service(guard, clock, {50});

  EXPECT_TRUE(service.requestCurrentData(0x05) == ObdServiceStatus::InProgress);
  clock.advanceMs(100);
  EXPECT_TRUE(service.poll() == ObdServiceStatus::InProgress);
  transport.pollStatus = TransportStatus::Complete;
  EXPECT_TRUE(service.poll() == ObdServiceStatus::InProgress);
  clock.advanceMs(49);
  EXPECT_TRUE(service.poll() == ObdServiceStatus::InProgress);
  clock.advanceMs(1);
  EXPECT_TRUE(service.poll() == ObdServiceStatus::Timeout);
}

inline void testObdSupportedPidBlocks() {
  RecordingDiagnosticTransport transport;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  ObdService service(guard, clock, {50});

  EXPECT_TRUE(service.requestSupportedPids(0x01) == ObdServiceStatus::InvalidRequest);
  EXPECT_TRUE(transport.sendCount == 0);
  EXPECT_TRUE(service.requestSupportedPids(0x00) == ObdServiceStatus::InProgress);
  EXPECT_TRUE(transport.lastPayload[0] == 0x01 && transport.lastPayload[1] == 0x00);
  transport.setResponse({0x41, 0x00, 0xBE, 0xEF, 0x00, 0x01});
  EXPECT_TRUE(service.poll() == ObdServiceStatus::ResponseReady);
  EXPECT_TRUE(service.supportedPidBitmap() == 0xBEEF0001U);

  EXPECT_TRUE(service.requestSupportedPids(0x20) == ObdServiceStatus::InProgress);
  transport.setResponse({0x41, 0x20, 0x00, 0x00, 0x00, 0x00});
  EXPECT_TRUE(service.poll() == ObdServiceStatus::ResponseReady);

  EXPECT_TRUE(service.requestSupportedPids(0x40) == ObdServiceStatus::InProgress);
  transport.setResponse({0x41, 0x40, 0x00, 0x00, 0x00});
  const auto malformedSupportedPidStatus = service.poll();
  EXPECT_TRUE(malformedSupportedPidStatus == ObdServiceStatus::InvalidResponse);
}

inline void testObdVinValidation() {
  RecordingDiagnosticTransport transport;
  FakeClock clock;
  ReadOnlyGuard guard(transport);
  ObdService service(guard, clock, {50});

  EXPECT_TRUE(service.requestVin() == ObdServiceStatus::InProgress);
  EXPECT_TRUE(transport.lastPayload[0] == 0x09 && transport.lastPayload[1] == 0x02);
  transport.setResponse({0x49, 0x02, 0x02, 'W', 'V', 'W', 'Z', 'Z', 'Z',
                         '1', 'K', 'Z', 'W', '0', '0', '0', '0', '0', '0', '1'});
  const auto wrongNodiStatus = service.poll();
  EXPECT_TRUE(wrongNodiStatus == ObdServiceStatus::InvalidResponse);

  EXPECT_TRUE(service.requestVin() == ObdServiceStatus::InProgress);
  transport.setResponse({0x49, 0x02, 0x01, 'W', 'V', 'W'});
  const auto shortVinStatus = service.poll();
  EXPECT_TRUE(shortVinStatus == ObdServiceStatus::InvalidResponse);

  EXPECT_TRUE(service.requestVin() == ObdServiceStatus::InProgress);
  transport.setResponse({0x49, 0x02, 0x01, 'W', 'V', 'W', 'Z', 'Z', 'Z',
                         '1', 'K', 'Z', 'W', '0', '0', '0', '0', '0', '0', '1',
                         'X'});
  const auto longVinStatus = service.poll();
  EXPECT_TRUE(longVinStatus == ObdServiceStatus::InvalidResponse);

  EXPECT_TRUE(service.requestVin() == ObdServiceStatus::InProgress);
  transport.setResponse({0x49, 0x03, 0x01, 'W', 'V', 'W', 'Z', 'Z', 'Z',
                         '1', 'K', 'Z', 'W', '0', '0', '0', '0', '0', '0', '1'});
  EXPECT_TRUE(service.poll() == ObdServiceStatus::UnexpectedResponse);
  EXPECT_TRUE(service.isRequestActive());
  transport.setResponse({0x49, 0x02, 0x01, 'W', 'V', 'W', 'Z', 'Z', 'Z',
                         '1', 'K', 'Z', 'W', '0', '0', '0', '0', '0', '0', '1'});
  EXPECT_TRUE(service.poll() == ObdServiceStatus::ResponseReady);
  const std::array<char, ObdService::kVinLength> expected{{
      'W', 'V', 'W', 'Z', 'Z', 'Z', '1', 'K', 'Z', 'W', '0', '0', '0',
      '0', '0', '0', '1'}};
  EXPECT_TRUE(service.vin() == expected);
}

inline void runObdServiceTests() {
  testObdCurrentDataAndResponseMatching();
  testObdBusyTimeoutAndTransportFailures();
  testObdWaitsForTransportCompletionBeforeTimeout();
  testObdSupportedPidBlocks();
  testObdVinValidation();
}

}  // namespace vag_data::test
