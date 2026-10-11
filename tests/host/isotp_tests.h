#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "../../src/protocol/isotp_transport.h"
#include "../../src/protocol/obd_service.h"
#include "../../src/protocol/uds_service.h"
#include "fake_clock.h"
#include "mock_can.h"
#include "test_helpers.h"

namespace vag_data::test {

inline IsoTpConfig makeIsoTpConfig() {
  IsoTpConfig config;
  config.txId = 0x700;
  config.rxId = 0x708;
  config.maxPayload = 256;
  config.flowControlTimeoutMs = 50;
  config.consecutiveFrameTimeoutMs = 50;
  config.maxWaitFrames = 2;
  return config;
}

inline void initialize(MockCan& can) {
  EXPECT_TRUE(can.initialize(HardwareConfig{}) == CanStatus::Ok);
}

inline void testIsoTpSingleFrames() {
  MockCan can;
  FakeClock clock;
  initialize(can);
  IsoTpTransport transport(can, clock, makeIsoTpConfig());

  const auto rx = makeFrame(0x708, {0x03, 0x11, 0x22, 0x33});
  EXPECT_TRUE(transport.onCanFrame(rx) == TransportStatus::MessageReady);
  std::array<std::uint8_t, 7> output{};
  std::array<std::uint8_t, 2> undersized{};
  std::size_t length = 0;
  EXPECT_TRUE(transport.receive(undersized.data(), undersized.size(), length) ==
              TransportStatus::Overflow);
  EXPECT_TRUE(length == 3);
  EXPECT_TRUE(transport.receive(output.data(), output.size(), length) ==
              TransportStatus::Complete);
  EXPECT_TRUE(length == 3);
  EXPECT_TRUE(output[0] == 0x11 && output[1] == 0x22 && output[2] == 0x33);

  const auto maxRx = makeFrame(0x708, {0x07, 1, 2, 3, 4, 5, 6, 7});
  EXPECT_TRUE(transport.onCanFrame(maxRx) == TransportStatus::MessageReady);
  EXPECT_TRUE(transport.receive(output.data(), output.size(), length) ==
              TransportStatus::Complete);
  EXPECT_TRUE(length == 7 && output[6] == 7);

  const std::array<std::uint8_t, 7> maxPayload{{1, 2, 3, 4, 5, 6, 7}};
  EXPECT_TRUE(transport.startSend(maxPayload.data(), maxPayload.size()) ==
              TransportStatus::Complete);
  EXPECT_TRUE(can.capturedTx().size() == 1);
  EXPECT_TRUE(can.capturedTx()[0].id == 0x700);
  EXPECT_TRUE(can.capturedTx()[0].length == 8);
  EXPECT_TRUE(can.capturedTx()[0].payload[0] == 0x07);
  for (std::size_t i = 0; i < maxPayload.size(); ++i) {
    EXPECT_TRUE(can.capturedTx()[0].payload[i + 1] == maxPayload[i]);
  }
}

inline void testIsoTpMalformedAndRouting() {
  MockCan can;
  FakeClock clock;
  initialize(can);
  IsoTpTransport transport(can, clock, makeIsoTpConfig());

  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x709, {0x01, 0xAA})) ==
              TransportStatus::Ignored);
  EXPECT_TRUE(transport.onCanFrame(
                  makeFrame(0x708, {0x01, 0xAA}, CanFrameFormat::Extended)) ==
              TransportStatus::Ignored);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x00})) ==
              TransportStatus::InvalidFrame);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x04, 1, 2})) ==
              TransportStatus::InvalidFrame);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x10})) ==
              TransportStatus::InvalidFrame);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x10, 0x0A, 1})) ==
              TransportStatus::InvalidFrame);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x10, 0x07, 1})) ==
              TransportStatus::InvalidFrame);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x21, 1})) ==
              TransportStatus::UnexpectedFrame);

  const auto unrelated = makeFrame(0x321, {0x01, 0x99});
  can.injectRx(unrelated);
  EXPECT_TRUE(transport.poll() == TransportStatus::Idle);
  CanFrame retained;
  EXPECT_TRUE(can.receive(retained) == CanStatus::Ok);
  EXPECT_TRUE(sameFrame(retained, unrelated));

  auto invalidConfig = makeIsoTpConfig();
  invalidConfig.frameFormat = static_cast<CanFrameFormat>(2);
  IsoTpTransport invalidTransport(can, clock, invalidConfig);
  const std::uint8_t payload = 0xAA;
  EXPECT_TRUE(invalidTransport.startSend(&payload, 1) == TransportStatus::InvalidArgument);
}

inline void testIsoTpReceiveReassemblyAndSequenceWrap() {
  MockCan can;
  FakeClock clock;
  initialize(can);
  auto config = makeIsoTpConfig();
  config.rxBlockSize = 0;
  IsoTpTransport transport(can, clock, config);

  constexpr std::size_t payloadLength = 118;
  std::array<std::uint8_t, payloadLength> expected{};
  for (std::size_t i = 0; i < expected.size(); ++i) {
    expected[i] = static_cast<std::uint8_t>(i);
  }

  CanFrame first = makeFrame(0x708, {0x10, 0x76, 0, 1, 2, 3, 4, 5});
  EXPECT_TRUE(transport.onCanFrame(first) == TransportStatus::InProgress);
  EXPECT_TRUE(can.capturedTx().size() == 1);
  EXPECT_TRUE(can.capturedTx()[0].payload[0] == 0x30);

  std::size_t offset = 6;
  std::uint8_t sequence = 1;
  while (offset < expected.size()) {
    CanFrame consecutive;
    consecutive.id = 0x708;
    consecutive.length = static_cast<std::uint8_t>(
        1 + std::min<std::size_t>(7, expected.size() - offset));
    consecutive.payload[0] = static_cast<std::uint8_t>(0x20 | sequence);
    for (std::size_t i = 1; i < consecutive.length; ++i) {
      consecutive.payload[i] = expected[offset++];
    }
    const auto status = transport.onCanFrame(consecutive);
    EXPECT_TRUE(status == (offset == expected.size()
                               ? TransportStatus::MessageReady
                               : TransportStatus::InProgress));
    sequence = static_cast<std::uint8_t>((sequence + 1) & 0x0F);
  }

  std::array<std::uint8_t, payloadLength> output{};
  std::size_t length = 0;
  EXPECT_TRUE(transport.receive(output.data(), output.size(), length) ==
              TransportStatus::Complete);
  EXPECT_TRUE(output == expected);
}

inline void testIsoTpReceiveFailures() {
  MockCan can;
  FakeClock clock;
  initialize(can);
  auto config = makeIsoTpConfig();
  config.maxPayload = 20;
  IsoTpTransport transport(can, clock, config);

  EXPECT_TRUE(transport.onCanFrame(
                  makeFrame(0x708, {0x10, 0x15, 1, 2, 3, 4, 5, 6})) ==
              TransportStatus::Overflow);
  EXPECT_TRUE(can.capturedTx().back().payload[0] == 0x32);

  EXPECT_TRUE(transport.onCanFrame(
                  makeFrame(0x708, {0x10, 0x0A, 1, 2, 3, 4, 5, 6})) ==
              TransportStatus::InProgress);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x22, 7, 8, 9, 10})) ==
              TransportStatus::SequenceError);

  EXPECT_TRUE(transport.onCanFrame(
                  makeFrame(0x708, {0x10, 0x0A, 1, 2, 3, 4, 5, 6})) ==
              TransportStatus::InProgress);
  clock.advanceMs(50);
  EXPECT_TRUE(transport.poll() == TransportStatus::Timeout);
}

inline void startMultiFrame(IsoTpTransport& transport,
                            const std::array<std::uint8_t, 27>& payload) {
  EXPECT_TRUE(transport.startSend(payload.data(), payload.size()) ==
              TransportStatus::InProgress);
}

inline std::array<std::uint8_t, 27> makeMultiFramePayload() {
  std::array<std::uint8_t, 27> payload{};
  for (std::size_t i = 0; i < payload.size(); ++i) {
    payload[i] = static_cast<std::uint8_t>(0x40 + i);
  }
  return payload;
}

inline void testIsoTpTransmitBlockSizeAndStMin() {
  MockCan can;
  FakeClock clock;
  initialize(can);
  IsoTpTransport transport(can, clock, makeIsoTpConfig());
  const auto payload = makeMultiFramePayload();
  startMultiFrame(transport, payload);
  EXPECT_TRUE(can.capturedTx().size() == 1);
  EXPECT_TRUE(can.capturedTx()[0].payload[0] == 0x10);
  EXPECT_TRUE(can.capturedTx()[0].payload[1] == payload.size());

  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x30, 0x01, 0x05})) ==
              TransportStatus::InProgress);
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(can.capturedTx().size() == 2);
  EXPECT_TRUE(can.capturedTx()[1].payload[0] == 0x21);
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(can.capturedTx().size() == 2);

  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x30, 0x00, 0x05})) ==
              TransportStatus::InProgress);
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(can.capturedTx().size() == 2);
  clock.advanceMs(4);
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(can.capturedTx().size() == 2);
  clock.advanceMs(1);
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(can.capturedTx().size() == 3);
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(can.capturedTx().size() == 3);
  clock.advanceMs(5);
  EXPECT_TRUE(transport.poll() == TransportStatus::Complete);
  EXPECT_TRUE(can.capturedTx().size() == 4);

  std::size_t payloadOffset = 0;
  for (std::size_t i = 2; i < can.capturedTx()[0].length; ++i) {
    EXPECT_TRUE(can.capturedTx()[0].payload[i] == payload[payloadOffset++]);
  }
  for (std::size_t frameIndex = 1; frameIndex < can.capturedTx().size();
       ++frameIndex) {
    for (std::size_t i = 1; i < can.capturedTx()[frameIndex].length; ++i) {
      EXPECT_TRUE(can.capturedTx()[frameIndex].payload[i] ==
                  payload[payloadOffset++]);
    }
  }
  EXPECT_TRUE(payloadOffset == payload.size());
}

inline void testIsoTpCrossBlockTimingAndBusyRetry() {
  for (const auto fcDelay : {0, 4, 5, 8}) {
    MockCan can;
    FakeClock clock;
    initialize(can);
    IsoTpTransport transport(can, clock, makeIsoTpConfig());
    const auto payload = makeMultiFramePayload();
    startMultiFrame(transport, payload);
    EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x30, 1, 5})) ==
                TransportStatus::InProgress);
    // The first CF after the first FC is immediately eligible.
    EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
    EXPECT_TRUE(can.capturedTx().size() == 2);
    clock.advanceMs(fcDelay);
    EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x30, 1, 5})) ==
                TransportStatus::InProgress);
    if (fcDelay < 5) {
      EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
      EXPECT_TRUE(can.capturedTx().size() == 2);
      clock.advanceMs(5 - fcDelay);
    }
    can.setNextTxStatus(CanStatus::TxBusy);
    EXPECT_TRUE(transport.poll() == TransportStatus::TxBusy);
    EXPECT_TRUE(can.capturedTx().size() == 2);
    clock.advanceMs(2);
    EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
    EXPECT_TRUE(can.capturedTx().size() == 3);
    EXPECT_TRUE(can.capturedTx().back().payload[0] == 0x22);
    // Separation starts at successful retry, not the rejected HAL attempt.
    EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x30, 1, 5})) ==
                TransportStatus::InProgress);
    clock.advanceMs(4);
    EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
    EXPECT_TRUE(can.capturedTx().size() == 3);
    clock.advanceMs(1);
    EXPECT_TRUE(transport.poll() == TransportStatus::Complete);
    EXPECT_TRUE(can.capturedTx().size() == 4);
    // A new transfer must not inherit the previous CF time.
    startMultiFrame(transport, payload);
    EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x30, 1, 5})) ==
                TransportStatus::InProgress);
    EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
    EXPECT_TRUE(can.capturedTx().size() == 6);
  }
}

inline void testIsoTpStMinUsesSuccessfulHalReturnTime() {
  // Model a synchronous HAL whose successful call consumes deterministic time.
  class TimedCan final : public CanHal {
   public:
    TimedCan(MockCan& backend, FakeClock& clock) : backend_(backend), clock_(clock) {}
    CanStatus initialize(const HardwareConfig& config) override {
      return backend_.initialize(config);
    }
    CanStatus stop() override { return backend_.stop(); }
    CanStatus receive(CanFrame& frame) override { return backend_.receive(frame); }
    CanStatus send(const CanFrame& frame) override {
      const auto status = backend_.send(frame);
      if (status == CanStatus::Ok) clock_.advanceMs(successDelayMs);
      return status;
    }
    std::uint64_t successDelayMs{0};
   private:
    MockCan& backend_;
    FakeClock& clock_;
  };
  MockCan backend;
  FakeClock clock;
  TimedCan can(backend, clock);
  EXPECT_TRUE(can.initialize(HardwareConfig{}) == CanStatus::Ok);
  IsoTpTransport transport(can, clock, makeIsoTpConfig());
  const auto payload = makeMultiFramePayload();
  startMultiFrame(transport, payload);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x30, 1, 5})) ==
              TransportStatus::InProgress);
  can.successDelayMs = 4;
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(clock.nowMs() == 4 && backend.capturedTx().size() == 2);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x30, 0, 5})) ==
              TransportStatus::InProgress);
  clock.advanceMs(4);
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(backend.capturedTx().size() == 2);
  clock.advanceMs(1);
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(clock.nowMs() == 13 && backend.capturedTx().size() == 3);
  clock.advanceMs(4);
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(backend.capturedTx().size() == 3);
  clock.advanceMs(1);
  EXPECT_TRUE(transport.poll() == TransportStatus::Complete);
  EXPECT_TRUE(clock.nowMs() == 22 && backend.capturedTx().size() == 4);
}

inline void testIsoTpWaitCountBoundaries() {
  for (const auto limit : {0, 1, 2, 254, 255}) {
    MockCan can;
    FakeClock clock;
    initialize(can);
    auto config = makeIsoTpConfig();
    config.maxWaitFrames = static_cast<std::uint8_t>(limit);
    IsoTpTransport transport(can, clock, config);
    const auto payload = makeMultiFramePayload();
    startMultiFrame(transport, payload);
    for (int count = 0; count < limit; ++count) {
      EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x31, 0, 0})) ==
                  TransportStatus::InProgress);
    }
    EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x31, 0, 0})) ==
                TransportStatus::WaitLimitExceeded);
    EXPECT_TRUE(transport.poll() == TransportStatus::Idle);
    startMultiFrame(transport, payload);
    EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x30, 0, 0})) ==
                TransportStatus::InProgress);
    EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  }
}

// Supply one complete multiframe response without draining CAN HAL traffic.
template <std::size_t Length>
inline void injectIsoTpMessage(IsoTpTransport& transport,
                              const std::array<std::uint8_t, Length>& payload) {
  static_assert(Length > 7 && Length <= IsoTpTransport::kMaxPayload);
  auto first = makeFrame(0x708, {0x10, 0, 0, 0, 0, 0, 0, 0});
  first.payload[0] |= static_cast<std::uint8_t>(Length >> 8);
  first.payload[1] = static_cast<std::uint8_t>(Length);
  std::copy_n(payload.begin(), 6, first.payload.begin() + 2);
  EXPECT_TRUE(transport.onCanFrame(first) == TransportStatus::InProgress);
  std::size_t offset = 6;
  std::uint8_t sequence = 1;
  while (offset < Length) {
    CanFrame next;
    next.id = 0x708;
    const auto count = std::min<std::size_t>(7, Length - offset);
    next.length = static_cast<std::uint8_t>(count + 1);
    next.payload[0] = static_cast<std::uint8_t>(0x20 | sequence);
    std::copy_n(payload.begin() + offset, count, next.payload.begin() + 1);
    offset += count;
    EXPECT_TRUE(transport.onCanFrame(next) ==
                (offset == Length ? TransportStatus::MessageReady
                                  : TransportStatus::InProgress));
    sequence = static_cast<std::uint8_t>((sequence + 1) & 0x0F);
  }
}

inline void testIsoTpOversizedServiceRecoveryAndReceiveRetry() {
  MockCan can;
  FakeClock clock;
  initialize(can);
  IsoTpTransport transport(can, clock, makeIsoTpConfig());
  ReadOnlyGuard guard(transport);
  std::array<std::uint8_t, 65> oversized{};
  oversized[0] = 0x62;
  oversized[1] = 0xF1;
  oversized[2] = 0x90;
  // The generic receive contract remains retryable with a larger buffer.
  injectIsoTpMessage(transport, oversized);
  std::array<std::uint8_t, 64> small{};
  std::array<std::uint8_t, 65> large{};
  std::size_t length = 0;
  EXPECT_TRUE(guard.receive(small.data(), small.size(), length) == TransportStatus::Overflow);
  EXPECT_TRUE(length == 65);
  EXPECT_TRUE(guard.receive(large.data(), large.size(), length) == TransportStatus::Complete);
  EXPECT_TRUE(length == 65 && large == oversized);

  const auto unrelated = makeFrame(0x321, {1, 0xAA});
  can.injectRx(unrelated);
  UdsService uds(guard, clock, {50, 10, 2});
  EXPECT_TRUE(uds.requestReadDataByIdentifier(0xF190) == UdsServiceStatus::InProgress);
  EXPECT_TRUE(can.capturedTx().back().payload[1] == 0x22);
  injectIsoTpMessage(transport, oversized);
  EXPECT_TRUE(uds.poll() == UdsServiceStatus::TransportFailure);
  EXPECT_TRUE(uds.lastTransportStatus() == TransportStatus::Overflow);
  EXPECT_TRUE(!uds.isRequestActive());
  EXPECT_TRUE(guard.receive(large.data(), large.size(), length) == TransportStatus::NoData);
  EXPECT_TRUE(uds.requestReadDataByIdentifier(0xF190) == UdsServiceStatus::InProgress);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {4, 0x62, 0xF1, 0x90, 0xAA})) ==
              TransportStatus::MessageReady);
  EXPECT_TRUE(uds.poll() == UdsServiceStatus::ResponseReady);
  EXPECT_TRUE(uds.rawDataLength() == 1 && uds.rawData()[0] == 0xAA);

  ObdService obd(guard, clock, {50});
  EXPECT_TRUE(obd.requestCurrentData(0x0C) == ObdServiceStatus::InProgress);
  EXPECT_TRUE(can.capturedTx().back().payload[1] == 0x01);
  oversized[0] = 0x41;
  oversized[1] = 0x0C;
  injectIsoTpMessage(transport, oversized);
  EXPECT_TRUE(obd.poll() == ObdServiceStatus::TransportFailure);
  EXPECT_TRUE(obd.lastTransportStatus() == TransportStatus::Overflow);
  EXPECT_TRUE(!obd.isRequestActive());
  EXPECT_TRUE(obd.requestCurrentData(0x0C) == ObdServiceStatus::InProgress);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {4, 0x41, 0x0C, 0x1A, 0xF8})) ==
              TransportStatus::MessageReady);
  EXPECT_TRUE(obd.poll() == ObdServiceStatus::ResponseReady);
  EXPECT_TRUE(obd.rawDataLength() == 2 && obd.rawData()[1] == 0xF8);
  CanFrame retained;
  EXPECT_TRUE(can.receive(retained) == CanStatus::Ok);
  EXPECT_TRUE(sameFrame(retained, unrelated));
}

inline void testIsoTpDiscardIsLimitedToPendingMessage() {
  MockCan can;
  FakeClock clock;
  initialize(can);
  IsoTpTransport transport(can, clock, makeIsoTpConfig());
  ReadOnlyGuard guard(transport);
  const auto unrelated = makeFrame(0x321, {1, 0xAA});
  can.injectRx(unrelated);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {1, 0xBB})) ==
              TransportStatus::MessageReady);
  const auto txCount = can.capturedTx().size();
  guard.discardPendingMessage();
  guard.discardPendingMessage();
  std::array<std::uint8_t, 10> output{};
  std::size_t length = 99;
  EXPECT_TRUE(guard.receive(output.data(), output.size(), length) == TransportStatus::NoData);
  EXPECT_TRUE(length == 0 && can.capturedTx().size() == txCount);
  // Discard with no completed message must not abort an in-flight assembly.
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x10, 10, 1, 2, 3, 4, 5, 6})) ==
              TransportStatus::InProgress);
  const auto payload = makeMultiFramePayload();
  startMultiFrame(transport, payload);
  guard.discardPendingMessage();
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x21, 7, 8, 9, 10})) ==
              TransportStatus::MessageReady);
  EXPECT_TRUE(guard.receive(output.data(), output.size(), length) == TransportStatus::Complete);
  EXPECT_TRUE(length == 10 && output[0] == 1 && output[9] == 10);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x30, 0, 0})) ==
              TransportStatus::InProgress);
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(can.capturedTx().back().payload[0] == 0x21);
  CanFrame retained;
  EXPECT_TRUE(can.receive(retained) == CanStatus::Ok);
  EXPECT_TRUE(sameFrame(retained, unrelated));
}

inline void testIsoTpServiceTimeoutDiscardsCompletedMessage() {
  for (const bool useUds : {false, true}) {
    MockCan can;
    FakeClock clock;
    initialize(can);
    IsoTpTransport transport(can, clock, makeIsoTpConfig());
    ReadOnlyGuard guard(transport);
    ObdService obd(guard, clock, {50});
    UdsService uds(guard, clock, {50, 10, 2});
    if (useUds) {
      EXPECT_TRUE(uds.requestReadDataByIdentifier(0xF190) == UdsServiceStatus::InProgress);
    } else {
      EXPECT_TRUE(obd.requestCurrentData(0x0C) == ObdServiceStatus::InProgress);
    }
    const auto response = useUds ? makeFrame(0x708, {4, 0x62, 0xF1, 0x90, 0xAA})
                                : makeFrame(0x708, {3, 0x41, 0x0C, 0xAA});
    EXPECT_TRUE(transport.onCanFrame(response) == TransportStatus::MessageReady);
    clock.advanceMs(50);
    if (useUds) {
      EXPECT_TRUE(uds.poll() == UdsServiceStatus::Timeout);
      EXPECT_TRUE(uds.requestReadDataByIdentifier(0xF190) == UdsServiceStatus::InProgress);
      EXPECT_TRUE(transport.onCanFrame(response) == TransportStatus::MessageReady);
      EXPECT_TRUE(uds.poll() == UdsServiceStatus::ResponseReady);
    } else {
      EXPECT_TRUE(obd.poll() == ObdServiceStatus::Timeout);
      EXPECT_TRUE(obd.requestCurrentData(0x0C) == ObdServiceStatus::InProgress);
      EXPECT_TRUE(transport.onCanFrame(response) == TransportStatus::MessageReady);
      EXPECT_TRUE(obd.poll() == ObdServiceStatus::ResponseReady);
    }
  }
}

inline void testIsoTpSubMillisecondPolicy() {
  MockCan can;
  FakeClock clock;
  initialize(can);
  IsoTpTransport transport(can, clock, makeIsoTpConfig());
  const auto payload = makeMultiFramePayload();
  startMultiFrame(transport, payload);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x30, 0x00, 0xF1})) ==
              TransportStatus::InProgress);
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  const auto count = can.capturedTx().size();
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(can.capturedTx().size() == count);
  clock.advanceMs(1);
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(can.capturedTx().size() == count + 1);
}

inline void testIsoTpFlowControlFailures() {
  const auto payload = makeMultiFramePayload();

  {
    MockCan can;
    FakeClock clock;
    initialize(can);
    IsoTpTransport transport(can, clock, makeIsoTpConfig());
    startMultiFrame(transport, payload);
    EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x31, 0, 0})) ==
                TransportStatus::InProgress);
    clock.advanceMs(50);
    EXPECT_TRUE(transport.poll() == TransportStatus::Timeout);
  }
  {
    MockCan can;
    FakeClock clock;
    initialize(can);
    IsoTpTransport transport(can, clock, makeIsoTpConfig());
    startMultiFrame(transport, payload);
    EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x31, 0, 0})) ==
                TransportStatus::InProgress);
    EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x31, 0, 0})) ==
                TransportStatus::InProgress);
    EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x31, 0, 0})) ==
                TransportStatus::WaitLimitExceeded);
  }
  {
    MockCan can;
    FakeClock clock;
    initialize(can);
    IsoTpTransport transport(can, clock, makeIsoTpConfig());
    startMultiFrame(transport, payload);
    EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x32, 0, 0})) ==
                TransportStatus::FlowControlOverflow);
  }
  {
    MockCan can;
    FakeClock clock;
    initialize(can);
    IsoTpTransport transport(can, clock, makeIsoTpConfig());
    startMultiFrame(transport, payload);
    EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x30, 0, 0x80})) ==
                TransportStatus::InvalidFrame);
  }
}

inline void testIsoTpCanStatusPropagation() {
  const std::array<std::uint8_t, 1> payload{{0x55}};

  MockCan can;
  FakeClock clock;
  IsoTpTransport transport(can, clock, makeIsoTpConfig());
  EXPECT_TRUE(transport.startSend(payload.data(), payload.size()) ==
              TransportStatus::NotInitialized);
  initialize(can);

  can.setNextTxStatus(CanStatus::TxBusy);
  EXPECT_TRUE(transport.startSend(payload.data(), payload.size()) ==
              TransportStatus::TxBusy);
  can.setNextTxStatus(CanStatus::TxFailed);
  EXPECT_TRUE(transport.startSend(payload.data(), payload.size()) ==
              TransportStatus::TxFailed);
  can.setNextTxStatus(CanStatus::BusOff);
  EXPECT_TRUE(transport.startSend(payload.data(), payload.size()) ==
              TransportStatus::BusOff);

  const auto multiFramePayload = makeMultiFramePayload();
  startMultiFrame(transport, multiFramePayload);
  EXPECT_TRUE(transport.onCanFrame(makeFrame(0x708, {0x30, 0, 0})) ==
              TransportStatus::InProgress);
  can.setNextTxStatus(CanStatus::TxBusy);
  EXPECT_TRUE(transport.poll() == TransportStatus::TxBusy);
  const auto framesBeforeRetry = can.capturedTx().size();
  EXPECT_TRUE(transport.poll() == TransportStatus::InProgress);
  EXPECT_TRUE(can.capturedTx().size() == framesBeforeRetry + 1);
  EXPECT_TRUE(can.capturedTx().back().payload[0] == 0x21);
}

inline void runIsoTpTests() {
  testIsoTpSingleFrames();
  testIsoTpMalformedAndRouting();
  testIsoTpReceiveReassemblyAndSequenceWrap();
  testIsoTpReceiveFailures();
  testIsoTpTransmitBlockSizeAndStMin();
  testIsoTpCrossBlockTimingAndBusyRetry();
  testIsoTpStMinUsesSuccessfulHalReturnTime();
  testIsoTpWaitCountBoundaries();
  testIsoTpOversizedServiceRecoveryAndReceiveRetry();
  testIsoTpDiscardIsLimitedToPendingMessage();
  testIsoTpServiceTimeoutDiscardsCompletedMessage();
  testIsoTpSubMillisecondPolicy();
  testIsoTpFlowControlFailures();
  testIsoTpCanStatusPropagation();
}

}  // namespace vag_data::test
