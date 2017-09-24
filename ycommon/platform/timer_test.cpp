#include "ycommon/platform/timer.h"

#include <gtest/gtest.h>

#include "ycommon/platform/sleep.h"

namespace ycommon { namespace platform {

TEST(BasicTimerTest, InitializationTest) {
  Timer test_timer;
}

TEST(BasicTimerTest, EmptyInitTest) {
  Timer test_timer;

  EXPECT_EQ(0, test_timer.GetPulsedTimeMicro());
  EXPECT_EQ(0, test_timer.GetPulsedTimeMilli());
  EXPECT_EQ(0, test_timer.GetPulsedTimeSeconds());
  EXPECT_EQ(0.0f, test_timer.GetPulsedTimeMicroFloat());
  EXPECT_EQ(0.0f, test_timer.GetPulsedTimeMilliFloat());
  EXPECT_EQ(0.0f, test_timer.GetPulsedTimeSecondsFloat());

  EXPECT_EQ(0, test_timer.GetDiffTimeMicro());
  EXPECT_EQ(0, test_timer.GetDiffTimeMilli());
  EXPECT_EQ(0, test_timer.GetDiffTimeSeconds());
  EXPECT_EQ(0.0f, test_timer.GetDiffTimeMicroFloat());
  EXPECT_EQ(0.0f, test_timer.GetDiffTimeMilliFloat());
  EXPECT_EQ(0.0f, test_timer.GetDiffTimeSecondsFloat());
}

TEST(BasicTimerTest, StartEmptyTest) {
  Timer test_timer;
  test_timer.Start();

  EXPECT_EQ(0, test_timer.GetPulsedTimeMicro());
  EXPECT_EQ(0, test_timer.GetPulsedTimeMilli());
  EXPECT_EQ(0, test_timer.GetPulsedTimeSeconds());
  EXPECT_EQ(0.0f, test_timer.GetPulsedTimeMicroFloat());
  EXPECT_EQ(0.0f, test_timer.GetPulsedTimeMilliFloat());
  EXPECT_EQ(0.0f, test_timer.GetPulsedTimeSecondsFloat());

  EXPECT_EQ(0, test_timer.GetDiffTimeMicro());
  EXPECT_EQ(0, test_timer.GetDiffTimeMilli());
  EXPECT_EQ(0, test_timer.GetDiffTimeSeconds());
  EXPECT_EQ(0.0f, test_timer.GetDiffTimeMicroFloat());
  EXPECT_EQ(0.0f, test_timer.GetDiffTimeMilliFloat());
  EXPECT_EQ(0.0f, test_timer.GetDiffTimeSecondsFloat());
}

TEST(BasicTimerTest, StopClearedTest) {
  Timer test_timer;
  test_timer.Start();
  test_timer.Pulse();
  test_timer.Stop();

  EXPECT_EQ(0, test_timer.GetPulsedTimeMicro());
  EXPECT_EQ(0, test_timer.GetPulsedTimeMilli());
  EXPECT_EQ(0, test_timer.GetPulsedTimeSeconds());
  EXPECT_EQ(0.0f, test_timer.GetPulsedTimeMicroFloat());
  EXPECT_EQ(0.0f, test_timer.GetPulsedTimeMilliFloat());
  EXPECT_EQ(0.0f, test_timer.GetPulsedTimeSecondsFloat());

  EXPECT_EQ(0, test_timer.GetDiffTimeMicro());
  EXPECT_EQ(0, test_timer.GetDiffTimeMilli());
  EXPECT_EQ(0, test_timer.GetDiffTimeSeconds());
  EXPECT_EQ(0.0f, test_timer.GetDiffTimeMicroFloat());
  EXPECT_EQ(0.0f, test_timer.GetDiffTimeMilliFloat());
  EXPECT_EQ(0.0f, test_timer.GetDiffTimeSecondsFloat());
}

TEST(BasicTimerTest, PulseTest) {
  Timer test_timer;
  test_timer.Start();

  Sleep::MicroSleep(10);
  test_timer.Pulse();

  EXPECT_LT(0, test_timer.GetPulsedTimeMicro());
  EXPECT_LT(0, test_timer.GetDiffTimeMicro());
}

TEST(BasicTimerTest, PulseDiffTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::MicroSleep(10);
  test_timer.Pulse();
  Sleep::MicroSleep(10);
  test_timer.Pulse();

  EXPECT_LT(0, test_timer.GetPulsedTimeMicro());
  EXPECT_LT(0, test_timer.GetDiffTimeMicro());
  EXPECT_NE(test_timer.GetPulsedTimeMicro(), test_timer.GetDiffTimeMicro());
}

TEST(BasicTimerTest, GetsAreConstantTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::MicroSleep(10);
  test_timer.Pulse();

  const int64_t pulsed = test_timer.GetPulsedTimeMicro();
  EXPECT_EQ(pulsed, test_timer.GetPulsedTimeMicro());

  const int64_t diff_time = test_timer.GetDiffTimeMicro();
  EXPECT_EQ(diff_time, test_timer.GetDiffTimeMicro());

  Sleep::MicroSleep(15);
  test_timer.Pulse();
  EXPECT_NE(pulsed, test_timer.GetPulsedTimeMicro());
  EXPECT_NE(diff_time, test_timer.GetDiffTimeMicro());
}

TEST(BasicTimerTest, PulseConversionTest) {
  Timer test_timer;
  test_timer.Start();
  Sleep::SleepFloat(0.5f);
  test_timer.Pulse();
  Sleep::SleepFloat(2.0f);
  test_timer.Pulse();

  const int64_t pulsed_micro = test_timer.GetPulsedTimeMicro();
  const int64_t pulsed_milli = test_timer.GetPulsedTimeMilli();
  const int64_t pulsed_secs = test_timer.GetPulsedTimeSeconds();

  EXPECT_LE((pulsed_micro + 500) / 1000 - 1, pulsed_milli);
  EXPECT_GE((pulsed_micro + 500) / 1000 + 1, pulsed_milli);
  EXPECT_LE(pulsed_milli / 1000 - 1, pulsed_secs);
  EXPECT_GE(pulsed_milli / 1000 + 1, pulsed_secs);

  const float pulsed_micro_float = test_timer.GetPulsedTimeMicroFloat();
  const float pulsed_milli_float = test_timer.GetPulsedTimeMilliFloat();
  const float pulsed_secs_float = test_timer.GetPulsedTimeSecondsFloat();

  EXPECT_FLOAT_EQ(pulsed_micro_float / 1000.0f, pulsed_milli_float);
  EXPECT_FLOAT_EQ(pulsed_milli_float / 1000.0f, pulsed_secs_float);

  const int64_t diff_micro = test_timer.GetDiffTimeMicro();
  const int64_t diff_milli = test_timer.GetDiffTimeMilli();
  const int64_t diff_secs = test_timer.GetDiffTimeSeconds();

  EXPECT_LE((diff_micro + 500) / 1000 - 1, diff_milli);
  EXPECT_GE((diff_micro + 500) / 1000 + 1, diff_milli);
  EXPECT_LE((diff_milli + 500) / 1000 - 1, diff_secs);
  EXPECT_GE((diff_milli + 500) / 1000 + 1, diff_secs);

  const float diff_micro_float = test_timer.GetDiffTimeMicroFloat();
  const float diff_milli_float = test_timer.GetDiffTimeMilliFloat();
  const float diff_secs_float = test_timer.GetDiffTimeSecondsFloat();

  EXPECT_FLOAT_EQ(diff_micro_float / 1000.0f, diff_milli_float);
  EXPECT_FLOAT_EQ(diff_milli_float / 1000.0f, diff_secs_float);
}

}} // namespace ycommon { namespace platform {
