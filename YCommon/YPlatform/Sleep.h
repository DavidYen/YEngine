#ifndef YCOMMON_YPLATFORM_SLEEP_H
#define YCOMMON_YPLATFORM_SLEEP_H

namespace YCommon { namespace YPlatform {

namespace Sleep {
  void MicroSleep(uint64_t microseconds);
  void MilliSleep(uint64_t milliseconds);
  void Sleep(uint64_t seconds);

  void MicroSleepFloat(float microseconds);
  void MilliSleepFloat(float milliseconds);
  void SleepFloat(float seconds);
}

}} // namespace YCommon { namespace YPlatform {

#endif // YCOMMON_YPLATFORM_SLEEP_H
