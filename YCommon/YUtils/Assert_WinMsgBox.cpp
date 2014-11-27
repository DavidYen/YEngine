#include <YCommon/Headers/stdincludes.h>
#include "Assert.h"

#include <Windows.h>

#include <YCommon/YPlatform/PlatformHandle_Win.h>

namespace {
  HWND gMainWindowHandle = NULL;
}

namespace YCommon { namespace YUtils {

void Assert::Initialize(
    const YCommon::YPlatform::PlatformHandle& platform_handle) {
  const YPlatform::WinPlatformHandle& win_handle =
      reinterpret_cast<const YPlatform::WinPlatformHandle&>(platform_handle);
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
  (void) file;
  (void) line;
  (void) message;
}

}} // namespace YCommon { namespace YUtils {
