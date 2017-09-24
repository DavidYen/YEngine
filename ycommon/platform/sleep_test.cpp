#include "ycommon/platform/sleep.h"

#include <gtest/gtest.h>

#include "ycommon/platform/timer.h"

namespace ycommon { namespace platform {

TEST(BasicSleepTest, MicroSleepTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::MicroSleep(200);
  test_timer.Pulse();

  EXPECT_LE(150, test_timer.GetPulsedTimeMicro());
  EXPECT_GE(250, test_timer.GetPulsedTimeMicro());
}

TEST(BasicTimerTest, MilliSleepTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::MilliSleep(100);
  test_timer.Pulse();

  EXPECT_LE(80, test_timer.GetPulsedTimeMilli());
  EXPECT_GE(120, test_timer.GetPulsedTimeMilli());
}

TEST(BasicTimerTest, SleepTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::Sleep(1);
  test_timer.Pulse();

  EXPECT_EQ(1, test_timer.GetPulsedTimeSeconds());
}

TEST(BasicSleepTest, MicroSleepFloatTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::MicroSleepFloat(100.5);
  test_timer.Pulse();

  EXPECT_LE(90.0, test_timer.GetPulsedTimeMicroFloat());
  EXPECT_GE(115.f, test_timer.GetPulsedTimeMicroFloat());
}

TEST(BasicTimerTest, MilliSleepFloatTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::MilliSleepFloat(2.5f);
  test_timer.Pulse();

  EXPECT_LE(2.45f, test_timer.GetPulsedTimeMilliFloat());
  EXPECT_GE(2.55f, test_timer.GetPulsedTimeMilliFloat());
}

TEST(BasicTimerTest, SleepFloatTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::SleepFloat(1.5f);
  test_timer.Pulse();

  EXPECT_LE(1.45f, test_timer.GetPulsedTimeSecondsFloat());
  EXPECT_GE(1.55f, test_timer.GetPulsedTimeSecondsFloat());
}

}} // namespace ycommon { namespace platform {
