#include "ycommon/utils/assert.h"

#include <cstdio>
#include <Windows.h>

#include "ycommon/platform/platform_handle_win.h"

namespace {
  HWND gMainWindowHandle = NULL;
}

namespace ycommon { namespace utils {

void Assert::Initialize(
    const platform::PlatformHandle& platform_handle) {
  const platform::WinPlatformHandle& win_handle =
      reinterpret_cast<const platform::WinPlatformHandle&>(platform_handle);
  gMainWindowHandle = win_handle.mHwnd;
}

void Assert::Terminate() {
}

void Assert::Assert(const char* file, uint32_t line, const char* message) {
  char buffer[512];
  buffer[sizeof(buffer)-1] = '\0';
  snprintf(buffer, sizeof(buffer)-1, "[%s:%u] %s", file, line, message);
  MessageBoxA(gMainWindowHandle, buffer, "Assertion", MB_OK );
  __debugbreak();
}

void Assert::Warn(const char* file, uint32_t line, const char* message) {
  static_cast<void>(file);
  static_cast<void>(line);
  static_cast<void>(message);
}

}} // namespace ycommon { namespace utils {
