#include <YCommon/Headers/stdincludes.h>
#include "Sleep.h"

#include <gtest/gtest.h>

#include "Timer.h"

namespace YCommon { namespace YPlatform {

TEST(BasicSleepTest, MicroSleepTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::MicroSleep(20);
  test_timer.Pulse();

  EXPECT_LE(19, test_timer.GetPulsedTimeMicro());
}

TEST(BasicTimerTest, MilliSleepTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::MilliSleep(10);
  test_timer.Pulse();

  EXPECT_LE(9, test_timer.GetPulsedTimeMilli());
}

TEST(BasicTimerTest, SleepTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::Sleep(1);
  test_timer.Pulse();

  EXPECT_LE(1, test_timer.GetPulsedTimeSeconds());
}

TEST(BasicSleepTest, MicroSleepFloatTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::MicroSleepFloat(10.5);
  test_timer.Pulse();

  EXPECT_LE(10.45f, test_timer.GetPulsedTimeMicroFloat());
}

TEST(BasicTimerTest, MilliSleepFloatTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::MilliSleepFloat(2.5f);
  test_timer.Pulse();

  EXPECT_LE(2.45f, test_timer.GetPulsedTimeMilliFloat());
}

TEST(BasicTimerTest, SleepFloatTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::SleepFloat(1.5f);
  test_timer.Pulse();

  EXPECT_LE(1.45f, test_timer.GetPulsedTimeSecondsFloat());
}

}} // namespace YCommon { namespace YPlatform {
