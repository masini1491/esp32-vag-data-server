#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "../../src/protocol/isotp_transport.h"
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
  testIsoTpSubMillisecondPolicy();
  testIsoTpFlowControlFailures();
  testIsoTpCanStatusPropagation();
}

}  // namespace vag_data::test
