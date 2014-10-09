#include <YEngine/stdincludes.h>
#include <Windows.h>

#include <YEngine/YFramework/PlatformHandle_Win.h>
#include <YEngine/YFramework/YFramework.h>

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
  wndclass.hIconSm = LoadIcon( NULL, IDI_APPLICATION );
  wndclass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
  wndclass.cbSize = sizeof(wndclass);
  wndclass.lpfnWndProc = YEngine::YFramework::WinMsgProc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hInstance = hInst;
  wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
  wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName =  TEXT("Test Dialog");
  wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;

  ShowCursor(false);

  if ( RegisterClassEx( &wndclass ) == 0 ) {
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

  if ( !hWnd ) {
    return 1;
  }

  YEngine::YFramework::PlatformHandle platform_handle;
  YEngine::YFramework::InitializePlatformHandle(platform_handle, hWnd);

  YEngine::YFramework::YFramework framework(&platform_handle);
  framework.Run();
  return 0;
}
