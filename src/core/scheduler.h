#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

#include "clock.h"

namespace vag_data {

// Opaque caller-supplied identity; the scheduler defines no job catalog.
using SchedulerJobId = std::uint32_t;

enum class SchedulePolicy : std::uint8_t { Startup, Periodic, OnDemand };

enum class SchedulerResult : std::uint8_t {
  Registered,
  DuplicateId,
  Full,
  InvalidConfig,
  TimeOverflow,
  Requested,
  AlreadyPending,
  NotFound,
  NotOnDemand,
  Taken,
  NoDueJob,
  Busy,
  Completed,
  NoActiveJob,
  WrongJob,
};

// Single-owner / externally serialized. Callers execute and complete jobs.
// Clock must remain monotonic for the scheduler's lifetime.
template <std::size_t Capacity>
class Scheduler {
 public:
  static_assert(Capacity > 0, "Scheduler capacity must be positive");

  explicit Scheduler(Clock& clock) : clock_(clock) {}

  SchedulerResult registerJob(SchedulerJobId id, SchedulePolicy policy,
                              std::uint64_t periodMs = 0) {
    if ((policy != SchedulePolicy::Startup &&
         policy != SchedulePolicy::Periodic &&
         policy != SchedulePolicy::OnDemand) ||
        (policy == SchedulePolicy::Periodic ? periodMs == 0 : periodMs != 0)) {
      return SchedulerResult::InvalidConfig;
    }
    if (find(id) != size_) {
      return SchedulerResult::DuplicateId;
    }
    if (size_ == Capacity) {
      return SchedulerResult::Full;
    }
    std::uint64_t deadline = 0;
    if (policy == SchedulePolicy::Periodic) {
      const auto now = clock_.nowMs();
      if (periodMs > kMaxTime - now) {
        return SchedulerResult::TimeOverflow;
      }
      deadline = now + periodMs;
    }
    jobs_[size_++] = {id, policy, periodMs, deadline,
                     policy == SchedulePolicy::Startup};
    return SchedulerResult::Registered;
  }

  SchedulerResult request(SchedulerJobId id) {
    const auto index = find(id);
    if (index == size_) {
      return SchedulerResult::NotFound;
    }
    auto& job = jobs_[index];
    if (job.policy != SchedulePolicy::OnDemand) {
      return SchedulerResult::NotOnDemand;
    }
    if (index == activeIndex_ || job.pending) {
      return SchedulerResult::AlreadyPending;
    }
    job.pending = true;
    return SchedulerResult::Requested;
  }

  SchedulerResult takeNextDue(SchedulerJobId& id) {
    if (hasActiveJob()) {
      return SchedulerResult::Busy;
    }
    const auto now = clock_.nowMs();
    for (std::size_t index = 0; index < size_; ++index) {
      const auto& job = jobs_[index];
      const bool due = job.policy == SchedulePolicy::Periodic
                           ? now >= job.deadlineMs
                           : job.pending;
      if (due) {
        activeIndex_ = index;
        id = job.id;
        return SchedulerResult::Taken;
      }
    }
    return SchedulerResult::NoDueJob;
  }

  SchedulerResult complete(SchedulerJobId id) {
    if (!hasActiveJob()) {
      return SchedulerResult::NoActiveJob;
    }
    auto& job = jobs_[activeIndex_];
    if (job.id != id) {
      return SchedulerResult::WrongJob;
    }
    if (job.policy == SchedulePolicy::Periodic) {
      const auto elapsedPeriods =
          (clock_.nowMs() - job.deadlineMs) / job.periodMs;
      // O(1) missed-period skipping; fail without mutation if no future
      // deadline fits the 64-bit monotonic time domain.
      if (elapsedPeriods >= (kMaxTime - job.deadlineMs) / job.periodMs) {
        return SchedulerResult::TimeOverflow;
      }
      job.deadlineMs += (elapsedPeriods + 1) * job.periodMs;
    } else {
      job.pending = false;
    }
    activeIndex_ = Capacity;
    return SchedulerResult::Completed;
  }

  bool hasActiveJob() const { return activeIndex_ != Capacity; }
  std::size_t size() const { return size_; }
  static constexpr std::size_t capacity() { return Capacity; }

 private:
  struct Job {
    SchedulerJobId id{0};
    SchedulePolicy policy{SchedulePolicy::OnDemand};
    std::uint64_t periodMs{0};
    std::uint64_t deadlineMs{0};
    bool pending{false};
  };

  std::size_t find(SchedulerJobId id) const {
    for (std::size_t index = 0; index < size_; ++index) {
      if (jobs_[index].id == id) {
        return index;
      }
    }
    return size_;
  }

  static constexpr auto kMaxTime = std::numeric_limits<std::uint64_t>::max();
  Clock& clock_;
  std::array<Job, Capacity> jobs_{};
  std::size_t size_{0};
  std::size_t activeIndex_{Capacity};
};

}  // namespace vag_data
