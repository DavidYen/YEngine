#include <YCommon/Headers/stdincludes.h>
#include "Sleep.h"

#include <Windows.h>

namespace YCommon { namespace YPlatform {

void Sleep::MicroSleep(int64_t microseconds) {
  LARGE_INTEGER frequency, start, current_time;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&start);

  for (;;) {
    QueryPerformanceCounter(&current_time);
    const int64_t diff = current_time.QuadPart - start.QuadPart;
    const int64_t diff_micro = diff * 1000 * 1000;
    if ((diff_micro / frequency.QuadPart) >= microseconds)
      break;
  }
}

void Sleep::MilliSleep(int64_t milliseconds) {
  ::Sleep(static_cast<DWORD>(milliseconds));
}

void Sleep::Sleep(int64_t seconds) {
  ::Sleep(static_cast<DWORD>(seconds * 1000));
}

void Sleep::MicroSleepFloat(float microseconds) {
  return MicroSleep(static_cast<int64_t>(microseconds + 0.5f));
}

void Sleep::MilliSleepFloat(float milliseconds) {
  return MicroSleep(static_cast<int64_t>(milliseconds * 1000.0f));
}

void Sleep::SleepFloat(float seconds) {
  return MilliSleep(static_cast<int64_t>(seconds * 1000.0f));
}

}} // namespace YCommon { namespace YPlatform {
