#ifndef YCOMMON_PLATFORM_SLEEP_H
#define YCOMMON_PLATFORM_SLEEP_H

#include <stdint.h>

namespace ycommon { namespace platform {

namespace Sleep {
  void MicroSleep(int64_t microseconds);
  void MilliSleep(int64_t milliseconds);
  void Sleep(int64_t seconds);

  void MicroSleepFloat(float microseconds);
  void MilliSleepFloat(float milliseconds);
  void SleepFloat(float seconds);
}

}} // namespace ycommon { namespace platform {

#endif // YCOMMON_PLATFORM_SLEEP_H
