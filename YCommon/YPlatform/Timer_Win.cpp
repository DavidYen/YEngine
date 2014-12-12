#include <YCommon/Headers/stdincludes.h>
#include "Timer.h"

#include <Windows.h>

namespace YCommon { namespace YPlatform {

struct WindowsPimpl {
  int64_t counts_per_seconds;
  int64_t start_count;
  int64_t last_pulsed_count;
  int64_t pulsed_count;
};

Timer::Timer() {
  static_assert(sizeof(WindowsPimpl) <= sizeof(mPimpl),
                "Windows Thread Pimpl larger than Thread Pimpl!");

  memset(mPimpl, 0, sizeof(mPimpl));

  WindowsPimpl* win_pimpl = reinterpret_cast<WindowsPimpl*>(mPimpl);
  LARGE_INTEGER frequency;
  BOOL ret_value = QueryPerformanceFrequency(&frequency);
  (void) ret_value;
  YASSERT(ret_value, "Could not query performance frequency.");

  win_pimpl->counts_per_seconds = frequency.QuadPart;
}

Timer::~Timer() {
}

void Timer::Start() {
  LARGE_INTEGER current_count;
  BOOL ret_value = QueryPerformanceCounter(&current_count);
  (void) ret_value;
  YASSERT(ret_value, "Could not query performance counter.");

  WindowsPimpl* win_pimpl = reinterpret_cast<WindowsPimpl*>(mPimpl);
  win_pimpl->start_count = current_count.QuadPart;
  win_pimpl->last_pulsed_count = current_count.QuadPart;
  win_pimpl->pulsed_count = current_count.QuadPart;
}

void Timer::Pulse() {
  LARGE_INTEGER current_count;
  BOOL ret_value = QueryPerformanceCounter(&current_count);
  (void) ret_value;
  YASSERT(ret_value, "Could not query performance counter.");

  WindowsPimpl* win_pimpl = reinterpret_cast<WindowsPimpl*>(mPimpl);
  win_pimpl->last_pulsed_count = win_pimpl->pulsed_count;
  win_pimpl->pulsed_count = current_count.QuadPart;
}

void Timer::Stop() {
  WindowsPimpl* win_pimpl = reinterpret_cast<WindowsPimpl*>(mPimpl);
  win_pimpl->start_count = 0;
  win_pimpl->last_pulsed_count = 0;
  win_pimpl->pulsed_count = 0;
}

// Get the pulsed time from start time.
int64_t Timer::GetPulsedTimeMicro() const {
  const WindowsPimpl* win_pimpl = reinterpret_cast<const WindowsPimpl*>(mPimpl);
  const int64_t diff = win_pimpl->pulsed_count - win_pimpl->start_count;
  const int64_t diff_micro = diff * 1000 * 1000;
  const int64_t counts_per_sec = win_pimpl->counts_per_seconds;
  const int64_t half_second = counts_per_sec / 2;
  const int64_t micro_seconds = (diff_micro + half_second) / counts_per_sec;

  return micro_seconds;
}

int64_t Timer::GetPulsedTimeMilli() const {
  const WindowsPimpl* win_pimpl = reinterpret_cast<const WindowsPimpl*>(mPimpl);
  const int64_t diff = win_pimpl->pulsed_count - win_pimpl->start_count;
  const int64_t diff_milli = diff * 1000;
  const int64_t counts_per_sec = win_pimpl->counts_per_seconds;
  const int64_t half_second = counts_per_sec / 2;
  const int64_t milli_seconds = (diff_milli + half_second) / counts_per_sec;

  return milli_seconds;
}

int64_t Timer::GetPulsedTimeSeconds() const {
  const WindowsPimpl* win_pimpl = reinterpret_cast<const WindowsPimpl*>(mPimpl);
  const int64_t diff = win_pimpl->pulsed_count - win_pimpl->start_count;
  const int64_t counts_per_sec = win_pimpl->counts_per_seconds;
  const int64_t half_second = counts_per_sec / 2;
  const int64_t seconds = (diff + half_second) / counts_per_sec;

  return seconds;
}

float Timer::GetPulsedTimeMicroFloat() const {
  const int64_t micro_time = GetPulsedTimeMicro();
  return static_cast<float>(micro_time);
}

float Timer::GetPulsedTimeMilliFloat() const {
  const int64_t micro_time = GetPulsedTimeMicro();
  return static_cast<float>(micro_time) / 1000.0f;
}

float Timer::GetPulsedTimeSecondsFloat() const {
  const int64_t micro_time = GetPulsedTimeMicro();
  return static_cast<float>(micro_time) / (1000.0f * 1000.0f);
}

// Get the pulsed time from the previous pulsed time.
int64_t Timer::GetDiffTimeMicro() const {
  const WindowsPimpl* win_pimpl = reinterpret_cast<const WindowsPimpl*>(mPimpl);
  const int64_t diff = win_pimpl->pulsed_count - win_pimpl->last_pulsed_count;
  const int64_t diff_micro = diff * 1000 * 1000;
  const int64_t counts_per_sec = win_pimpl->counts_per_seconds;
  const int64_t half_second = counts_per_sec / 2;
  const int64_t micro_seconds = (diff_micro + half_second) / counts_per_sec;

  return micro_seconds;
}

int64_t Timer::GetDiffTimeMilli() const {
  const WindowsPimpl* win_pimpl = reinterpret_cast<const WindowsPimpl*>(mPimpl);
  const int64_t diff = win_pimpl->pulsed_count - win_pimpl->last_pulsed_count;
  const int64_t diff_milli = diff * 1000;
  const int64_t counts_per_sec = win_pimpl->counts_per_seconds;
  const int64_t half_second = counts_per_sec / 2;
  const int64_t milli_seconds = (diff_milli + half_second) / counts_per_sec;

  return milli_seconds;
}

int64_t Timer::GetDiffTimeSeconds() const {
  const WindowsPimpl* win_pimpl = reinterpret_cast<const WindowsPimpl*>(mPimpl);
  const int64_t diff = win_pimpl->pulsed_count - win_pimpl->last_pulsed_count;
  const int64_t counts_per_sec = win_pimpl->counts_per_seconds;
  const int64_t half_second = counts_per_sec / 2;
  const int64_t seconds = (diff + half_second) / counts_per_sec;

  return seconds;
}

float Timer::GetDiffTimeMicroFloat() const {
  const int64_t micro_time = GetDiffTimeMicro();
  return static_cast<float>(micro_time);
}

float Timer::GetDiffTimeMilliFloat() const {
  const int64_t micro_time = GetDiffTimeMicro();
  return static_cast<float>(micro_time) / 1000.0f;
}

float Timer::GetDiffTimeSecondsFloat() const {
  const int64_t micro_time = GetDiffTimeMicro();
  return static_cast<float>(micro_time) / (1000.0f * 1000.0f);
}

}} // namespace YCommon { namespace YPlatform {
