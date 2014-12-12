#ifndef YCOMMON_YPLATFORM_TIMER_H
#define YCOMMON_YPLATFORM_TIMER_H

namespace YCommon { namespace YPlatform {

class Timer {
 public:
  Timer();
  ~Timer();

  void Start();
  void Pulse();
  void Stop();

  // Get the pulsed time from start time.
  int64_t GetPulsedTimeMicro() const;
  int64_t GetPulsedTimeMilli() const;
  int64_t GetPulsedTimeSeconds() const;
  float GetPulsedTimeMicroFloat() const;
  float GetPulsedTimeMilliFloat() const;
  float GetPulsedTimeSecondsFloat() const;

  // Get the pulsed time from the previous pulsed time.
  int64_t GetDiffTimeMicro() const;
  int64_t GetDiffTimeMilli() const;
  int64_t GetDiffTimeSeconds() const;
  float GetDiffTimeMicroFloat() const;
  float GetDiffTimeMilliFloat() const;
  float GetDiffTimeSecondsFloat() const;

 private:
  char mPimpl[32];
};

}} // namespace YCommon { namespace YPlatform {

#endif // YCOMMON_YPLATFORM_TIMER_H
