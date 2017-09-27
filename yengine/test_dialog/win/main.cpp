#include <Windows.h>

#include "ycommon/platform/platform_handle.h"
#include "ycommon/platform/platform_handle_win.h"
#include "ycommon/platform/platform_win.h"
#include "yengine/framework/framework.h"
#include "yengine/render_device/render_device.h"

// Windows Entry Point
int WINAPI WinMain(HINSTANCE hInst,
                   HINSTANCE /* hPrevInstance */,
                   LPSTR /* lpCmdLine */,
                   int /* nCmdShow */) {
  WNDCLASSEX wndclass;

  // Setup Windows Class
  wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wndclass.cbSize = sizeof(wndclass);
  wndclass.lpfnWndProc = ycommon::platform::Platform::WinMsgProc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hInstance = hInst;
  wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName =  TEXT("Test Dialog");
  wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;

  ShowCursor(false);

  if (RegisterClassEx( &wndclass ) == 0) {
    return 0;
  }

  HWND hWnd = CreateWindowEx(NULL,
                             TEXT("Test Dialog"),
                             TEXT("Testing Engine..."),
                             WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                             GetSystemMetrics(SM_CXSCREEN)/2 - 190,
                             GetSystemMetrics(SM_CYSCREEN)/2 - 140,
                             GetSystemMetrics(SM_CXSCREEN)/2,
                             GetSystemMetrics(SM_CYSCREEN)/2,
                             NULL,
                             NULL,
                             hInst,
                             NULL);

  if (!hWnd) {
    return 1;
  }

  ycommon::platform::PlatformHandle platform_handle;
  ycommon::platform::InitWinPlatformHandle(platform_handle, hWnd, true);

  static char render_buffer[1024*10];
  yengine::framework::Framework framework(platform_handle);
  yengine::render_device::RenderDevice::Initialize(platform_handle,
                                                   1280,
                                                   720,
                                                   render_buffer,
                                                   sizeof(render_buffer));

  framework.Run();
  return 0;
}
