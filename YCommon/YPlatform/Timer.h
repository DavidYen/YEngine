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
  uint64_t GetPulsedTimeMicro() const;
  uint64_t GetPulsedTimeMilli() const;
  uint64_t GetPulsedTimeSeconds() const;
  float GetPulsedTimeMicroFloat() const;
  float GetPulsedTimeMilliFloat() const;
  float GetPulsedTimeSecondsFloat() const;

  // Get the pulsed time from the previous pulsed time.
  uint64_t GetDiffTimeMicro() const;
  uint64_t GetDiffTimeMilli() const;
  uint64_t GetDiffTimeSeconds() const;
  float GetDiffTimeMicroFloat() const;
  float GetDiffTimeMilliFloat() const;
  float GetDiffTimeSecondsFloat() const;

 private:
  char mPimpl[32];
};

}} // namespace YCommon { namespace YPlatform {

#endif // YCOMMON_YPLATFORM_TIMER_H
