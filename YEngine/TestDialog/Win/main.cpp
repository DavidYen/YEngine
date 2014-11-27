#include <YCommon/Headers/stdincludes.h>
#include <Windows.h>

#include <YCommon/YPlatform/Platform_Win.h>
#include <YCommon/YPlatform/PlatformHandle.h>
#include <YCommon/YPlatform/PlatformHandle_Win.h>
#include <YEngine/YFramework/YFramework.h>
#include <YEngine/YRenderDevice/RenderDevice.h>

// Windows Entry Point
int WINAPI WinMain(HINSTANCE hInst,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {
  (void) hPrevInstance;
  (void) lpCmdLine;
  (void) nCmdShow;
  WNDCLASSEX wndclass;

  // Setup Windows Class
  wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wndclass.cbSize = sizeof(wndclass);
  wndclass.lpfnWndProc = YCommon::YPlatform::Platform::WinMsgProc;
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

  YCommon::YPlatform::PlatformHandle platform_handle;
  YCommon::YPlatform::InitWinPlatformHandle(platform_handle, hWnd, true);

  static char render_buffer[1024];
  YEngine::YFramework::YFramework framework(platform_handle);
  YEngine::YRenderDevice::RenderDevice::Initialize(platform_handle,
                                                   1280,
                                                   720,
                                                   render_buffer,
                                                   sizeof(render_buffer));

  framework.Run();
  return 0;
}
