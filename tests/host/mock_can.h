#pragma once

#include <deque>
#include <utility>
#include <vector>

#include "../../src/hal/can_hal.h"

namespace vag_data::test {

class MockCan final : public CanHal {
 public:
  CanStatus initialize(const HardwareConfig&) override {
    initialized_ = !failInitialization_;
    return initialized_ ? CanStatus::Ok : CanStatus::InvalidConfig;
  }

  CanStatus stop() override {
    initialized_ = false;
    rxFrames_.clear();
    return CanStatus::Ok;
  }

  CanStatus send(const CanFrame& frame) override {
    if (!initialized_) {
      return CanStatus::NotInitialized;
    }
    if (!frame.isValid()) {
      return CanStatus::InvalidConfig;
    }
    if (failNextTx_) {
      failNextTx_ = false;
      return CanStatus::TxFailed;
    }
    txFrames_.push_back(frame);
    return CanStatus::Ok;
  }

  CanStatus receive(CanFrame& frame) override {
    if (!initialized_) {
      return CanStatus::NotInitialized;
    }
    if (rxFrames_.empty()) {
      return CanStatus::NoData;
    }
    frame = rxFrames_.front();
    rxFrames_.pop_front();
    return CanStatus::Ok;
  }

  void injectRx(const CanFrame& frame) { rxFrames_.push_back(frame); }

  const std::vector<CanFrame>& capturedTx() const { return txFrames_; }

  void clear() {
    rxFrames_.clear();
    txFrames_.clear();
    failNextTx_ = false;
    failInitialization_ = false;
    initialized_ = false;
  }

  void failNextTx() { failNextTx_ = true; }
  void failInitialization() { failInitialization_ = true; }

 private:
  bool initialized_{false};
  bool failNextTx_{false};
  bool failInitialization_{false};
  std::deque<CanFrame> rxFrames_;
  std::vector<CanFrame> txFrames_;
};

}  // namespace vag_data::test
