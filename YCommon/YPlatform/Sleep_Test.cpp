#include <YCommon/Headers/stdincludes.h>
#include "Sleep.h"

#include <gtest/gtest.h>

#include "Timer.h"

namespace YCommon { namespace YPlatform {

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

  EXPECT_LE(90, test_timer.GetPulsedTimeMilli());
  EXPECT_GE(110, test_timer.GetPulsedTimeMilli());
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

}} // namespace YCommon { namespace YPlatform {
