#include <initializer_list>
#include <algorithm>

#include "fake_clock.h"
#include "mock_can.h"
#include "test_helpers.h"

namespace {

using namespace vag_data;
using namespace vag_data::test;

void testCanFrameModel() {
  static_assert(CanFrame::kClassicCanMaxPayload == 8);
  static_assert(CanStatus::TxBusy != CanStatus::TxFailed);

  const auto zeroId = makeFrame(0x000, {});
  const auto standardMax = makeFrame(0x7FF, {0xFF});
  const auto extendedZero = makeFrame(0x00000000, {}, CanFrameFormat::Extended);
  const auto extendedMax = makeFrame(0x1FFFFFFF, {0xAA}, CanFrameFormat::Extended);
  EXPECT_TRUE(zeroId.id == 0x000);
  EXPECT_TRUE(standardMax.id == 0x7FF);
  EXPECT_TRUE(extendedZero.id == 0x00000000);
  EXPECT_TRUE(extendedMax.id == 0x1FFFFFFF);
  EXPECT_TRUE(extendedZero.format == CanFrameFormat::Extended);
  EXPECT_TRUE(extendedMax.format == CanFrameFormat::Extended);
  EXPECT_TRUE(makeFrame(0x800, {}).isValid() == false);
  EXPECT_TRUE(makeFrame(0x20000000, {}, CanFrameFormat::Extended).isValid() == false);

  const auto frame = makeFrame(0x123, {0x01, 0x02, 0x03});
  EXPECT_TRUE(frame.id == 0x123);
  EXPECT_TRUE(frame.format == CanFrameFormat::Standard);
  EXPECT_TRUE(frame.length == 3);
  EXPECT_TRUE(frame.isValid());
  EXPECT_TRUE(makeFrame(0x100, {}).isValid());
  EXPECT_TRUE(makeFrame(0x100, {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}).isValid());

  CanFrame invalidLength;
  invalidLength.length = CanFrame::kClassicCanMaxPayload + 1;
  EXPECT_TRUE(!invalidLength.isValid());

  const auto malformed = makeFrame(0x100, {0x00, 0x01, 0x02, 0x03, 0x04,
                                           0x05, 0x06, 0x07, 0x08});
  EXPECT_TRUE(malformed.length == 9);
  EXPECT_TRUE(!malformed.isValid());
  EXPECT_TRUE(!sameFrame(malformed, malformed));

  auto timestamped = frame;
  timestamped.timestamp = 123456;
  const auto copied = timestamped;
  EXPECT_TRUE(copied.timestamp == 123456);
  EXPECT_TRUE(sameFrame(timestamped, copied));
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
  const auto invalidStandard = makeFrame(0x800, {});
  const auto invalidExtended = makeFrame(0x20000000, {}, CanFrameFormat::Extended);
  const auto invalidPayload = makeFrame(0x100, {0x00, 0x01, 0x02, 0x03, 0x04,
                                                0x05, 0x06, 0x07, 0x08});
  EXPECT_TRUE(mock.send(invalidStandard) == CanStatus::InvalidConfig);
  EXPECT_TRUE(mock.send(invalidExtended) == CanStatus::InvalidConfig);
  EXPECT_TRUE(mock.send(invalidPayload) == CanStatus::InvalidConfig);
  EXPECT_TRUE(mock.capturedTx().size() == 1);
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

  mock.injectRx(frame);
  EXPECT_TRUE(mock.stop() == CanStatus::Ok);
  EXPECT_TRUE(mock.send(frame) == CanStatus::NotInitialized);
  EXPECT_TRUE(mock.receive(received) == CanStatus::NotInitialized);

  EXPECT_TRUE(mock.initialize(HardwareConfig{}) == CanStatus::Ok);
  EXPECT_TRUE(mock.receive(received) == CanStatus::NoData);

  mock.clear();
  EXPECT_TRUE(mock.initialize(HardwareConfig{}) == CanStatus::Ok);
  mock.injectRx(frame);
  mock.failNextTx();
  mock.clear();
  EXPECT_TRUE(mock.receive(received) == CanStatus::NotInitialized);
  EXPECT_TRUE(mock.capturedTx().empty());
  EXPECT_TRUE(mock.initialize(HardwareConfig{}) == CanStatus::Ok);
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
