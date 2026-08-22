#include <initializer_list>
#include <algorithm>

#include "fake_clock.h"
#include "mock_can.h"
#include "test_helpers.h"

namespace {

using namespace vag_data;
using namespace vag_data::test;

void testCanFrameModel() {
  const auto frame = makeFrame(0x123, {0x01, 0x02, 0x03});
  EXPECT_TRUE(frame.id == 0x123);
  EXPECT_TRUE(frame.format == CanFrameFormat::Standard);
  EXPECT_TRUE(frame.length == 3);
  EXPECT_TRUE(frame.isValid());
  EXPECT_TRUE(makeFrame(0x18DAF110, {0xAA}, CanFrameFormat::Extended).format ==
              CanFrameFormat::Extended);
}

void testMockCanRxFifo() {
  MockCan mock;
  EXPECT_TRUE(mock.initialize(HardwareConfig{}) == CanStatus::Ok);
  CanFrame received;
  EXPECT_TRUE(mock.receive(received) == CanStatus::NoData);

  const auto first = makeFrame(1, {0x10});
  const auto second = makeFrame(2, {0x20});
  mock.injectRx(first);
  mock.injectRx(second);
  EXPECT_TRUE(mock.receive(received) == CanStatus::Ok);
  EXPECT_TRUE(sameFrame(received, first));
  EXPECT_TRUE(mock.receive(received) == CanStatus::Ok);
  EXPECT_TRUE(sameFrame(received, second));
  EXPECT_TRUE(mock.receive(received) == CanStatus::NoData);
}

void testMockCanTxAndReset() {
  MockCan mock;
  const auto frame = makeFrame(3, {0x30, 0x31});
  EXPECT_TRUE(mock.send(frame) == CanStatus::NotInitialized);
  EXPECT_TRUE(mock.initialize(HardwareConfig{}) == CanStatus::Ok);
  EXPECT_TRUE(mock.send(frame) == CanStatus::Ok);
  EXPECT_TRUE(mock.capturedTx().size() == 1);
  EXPECT_TRUE(sameFrame(mock.capturedTx().front(), frame));
  mock.clear();
  EXPECT_TRUE(mock.capturedTx().empty());
  CanFrame received;
  EXPECT_TRUE(mock.receive(received) == CanStatus::NotInitialized);

  mock.failInitialization();
  EXPECT_TRUE(mock.initialize(HardwareConfig{}) == CanStatus::InvalidConfig);
  mock.clear();
  EXPECT_TRUE(mock.initialize(HardwareConfig{}) == CanStatus::Ok);
  mock.failNextTx();
  EXPECT_TRUE(mock.send(frame) == CanStatus::TxFailed);
  EXPECT_TRUE(mock.capturedTx().empty());
  EXPECT_TRUE(mock.send(frame) == CanStatus::Ok);
}

void testFakeClock() {
  FakeClock clock;
  EXPECT_TRUE(clock.nowMs() == 0);
  clock.advanceMs(25);
  EXPECT_TRUE(clock.nowMs() == 25);
  clock.setMs(100);
  EXPECT_TRUE(clock.nowMs() == 100);
  clock.advanceMs(7);
  EXPECT_TRUE(clock.nowMs() == 107);
  clock.reset();
  EXPECT_TRUE(clock.nowMs() == 0);
}

}  // namespace

int main() {
  testCanFrameModel();
  testMockCanRxFifo();
  testMockCanTxAndReset();
  testFakeClock();
  return 0;
}
