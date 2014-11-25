#include <YEngine/stdincludes.h>
#include "YAssert.h"

#include <Windows.h>

#include <YEngine/YFramework/PlatformHandle_Win.h>

namespace {
  HWND gMainWindowHandle = NULL;
}

namespace YEngine { namespace YCore {

void YAssert::Initialize(
    const YEngine::YFramework::PlatformHandle& platform_handle) {
  const WinPlatformHandle& win_handle =
    reinterpret_cast<const WinPlatformHandle&>(platform_handle);
  gMainWindowHandle = win_handle.mHwnd;
}

void YAssert::Terminate() {
}

void YAssert::Assert(const char* file, uint32_t line, const char* message) {
  char buffer[512];
  buffer[sizeof(buffer)-1] = '\0';
  snprintf(buffer, sizeof(buffer)-1, "[%s:%u] %s", file, line, message);
  MessageBoxA(gMainWindowHandle, buffer, "Yengine Assertion", MB_OK );
  __debugbreak();
}

void YAssert::Warn(const char* file, uint32_t line, const char* message) {
  (void) file;
  (void) line;
  (void) message;
}

}} // namespace YEngine { namespace YCore {
