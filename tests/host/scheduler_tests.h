#pragma once

#include <limits>

#include "../../src/core/scheduler.h"
#include "fake_clock.h"
#include "test_helpers.h"

namespace vag_data::test {

inline void testSchedulerRegistrationAndStartup() {
  FakeClock clock;
  Scheduler<2> scheduler(clock);
  EXPECT_TRUE(scheduler.registerJob(9, SchedulePolicy::Periodic, 0) ==
              SchedulerResult::InvalidConfig);
  EXPECT_TRUE(scheduler.registerJob(9, SchedulePolicy::Startup, 1) ==
              SchedulerResult::InvalidConfig);
  EXPECT_TRUE(scheduler.registerJob(9, static_cast<SchedulePolicy>(255)) ==
              SchedulerResult::InvalidConfig);
  EXPECT_TRUE(scheduler.size() == 0);
  EXPECT_TRUE(scheduler.registerJob(2, SchedulePolicy::Startup) ==
              SchedulerResult::Registered);
  EXPECT_TRUE(scheduler.registerJob(2, SchedulePolicy::OnDemand) ==
              SchedulerResult::DuplicateId);
  EXPECT_TRUE(scheduler.registerJob(1, SchedulePolicy::Startup) ==
              SchedulerResult::Registered);
  EXPECT_TRUE(scheduler.registerJob(3, SchedulePolicy::Startup) ==
              SchedulerResult::Full);
  EXPECT_TRUE(scheduler.size() == 2);
  SchedulerJobId id = 99;
  EXPECT_TRUE(scheduler.complete(2) == SchedulerResult::NoActiveJob);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Taken);
  EXPECT_TRUE(id == 2);  // Insertion order, not numeric ID order.
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Busy);
  EXPECT_TRUE(id == 2);
  EXPECT_TRUE(scheduler.complete(1) == SchedulerResult::WrongJob);
  EXPECT_TRUE(scheduler.hasActiveJob());
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Busy);
  EXPECT_TRUE(scheduler.complete(2) == SchedulerResult::Completed);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Taken);
  EXPECT_TRUE(id == 1);
  EXPECT_TRUE(scheduler.complete(1) == SchedulerResult::Completed);
  clock.advanceMs(100000);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::NoDueJob);
  EXPECT_TRUE(!scheduler.hasActiveJob());
}

inline void testSchedulerPeriodicCadence() {
  FakeClock clock;
  const std::uint64_t base = 0x100000001ULL;
  clock.setMs(base);
  Scheduler<1> scheduler(clock);
  EXPECT_TRUE(scheduler.registerJob(7, SchedulePolicy::Periodic, 10) ==
              SchedulerResult::Registered);
  SchedulerJobId id = 0;
  clock.setMs(base + 9);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::NoDueJob);
  for (std::uint64_t cycle = 1; cycle <= 5; ++cycle) {
    clock.setMs(base + cycle * 10);
    EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Taken);
    EXPECT_TRUE(id == 7);
    clock.advanceMs(3);
    EXPECT_TRUE(scheduler.complete(7) == SchedulerResult::Completed);
    clock.setMs(base + (cycle + 1) * 10 - 1);
    EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::NoDueJob);
  }
  clock.setMs(base + 60);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Taken);
  clock.setMs(base + 106);  // Skip deadlines 70, 80, 90 and 100.
  EXPECT_TRUE(scheduler.complete(7) == SchedulerResult::Completed);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::NoDueJob);
  clock.setMs(base + 109);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::NoDueJob);
  clock.setMs(base + 110);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Taken);
  clock.setMs(base + 120);  // Completion exactly on a subsequent deadline.
  EXPECT_TRUE(scheduler.complete(7) == SchedulerResult::Completed);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::NoDueJob);
  clock.setMs(base + 130);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Taken);
}

inline void testSchedulerOnDemandAndMixedOrder() {
  FakeClock clock;
  Scheduler<3> scheduler(clock);
  EXPECT_TRUE(scheduler.registerJob(3, SchedulePolicy::OnDemand) ==
              SchedulerResult::Registered);
  EXPECT_TRUE(scheduler.registerJob(2, SchedulePolicy::Periodic, 5) ==
              SchedulerResult::Registered);
  EXPECT_TRUE(scheduler.registerJob(1, SchedulePolicy::Startup) ==
              SchedulerResult::Registered);
  EXPECT_TRUE(scheduler.request(99) == SchedulerResult::NotFound);
  EXPECT_TRUE(scheduler.request(2) == SchedulerResult::NotOnDemand);
  EXPECT_TRUE(scheduler.request(1) == SchedulerResult::NotOnDemand);
  SchedulerJobId id = 0;
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Taken);
  EXPECT_TRUE(id == 1);  // OnDemand starts dormant; Periodic not yet due.
  EXPECT_TRUE(scheduler.complete(1) == SchedulerResult::Completed);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::NoDueJob);
  EXPECT_TRUE(scheduler.request(3) == SchedulerResult::Requested);
  EXPECT_TRUE(scheduler.request(3) == SchedulerResult::AlreadyPending);
  clock.setMs(5);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Taken);
  EXPECT_TRUE(id == 3);  // Simultaneously due jobs use registration order.
  EXPECT_TRUE(scheduler.request(3) == SchedulerResult::AlreadyPending);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Busy);
  EXPECT_TRUE(scheduler.complete(2) == SchedulerResult::WrongJob);
  EXPECT_TRUE(scheduler.complete(3) == SchedulerResult::Completed);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Taken);
  EXPECT_TRUE(id == 2);
  EXPECT_TRUE(scheduler.complete(2) == SchedulerResult::Completed);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::NoDueJob);
  EXPECT_TRUE(scheduler.request(3) == SchedulerResult::Requested);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Taken);
  EXPECT_TRUE(id == 3);
  EXPECT_TRUE(scheduler.complete(3) == SchedulerResult::Completed);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::NoDueJob);
}

inline void testSchedulerTimeOverflow() {
  FakeClock clock;
  constexpr auto max = std::numeric_limits<std::uint64_t>::max();
  clock.setMs(max - 5);
  Scheduler<1> scheduler(clock);
  EXPECT_TRUE(scheduler.registerJob(1, SchedulePolicy::Periodic, 6) ==
              SchedulerResult::TimeOverflow);
  EXPECT_TRUE(scheduler.size() == 0);
  EXPECT_TRUE(scheduler.registerJob(1, SchedulePolicy::Periodic, 5) ==
              SchedulerResult::Registered);
  SchedulerJobId id = 0;
  clock.setMs(max - 1);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::NoDueJob);
  clock.setMs(max);
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Taken);
  EXPECT_TRUE(scheduler.complete(1) == SchedulerResult::TimeOverflow);
  EXPECT_TRUE(scheduler.hasActiveJob());
  EXPECT_TRUE(scheduler.takeNextDue(id) == SchedulerResult::Busy);
}

inline void runSchedulerTests() {
  testSchedulerRegistrationAndStartup();
  testSchedulerPeriodicCadence();
  testSchedulerOnDemandAndMixedOrder();
  testSchedulerTimeOverflow();
}

}  // namespace vag_data::test
