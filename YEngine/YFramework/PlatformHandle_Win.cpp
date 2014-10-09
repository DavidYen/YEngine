#include <YEngine/stdincludes.h>
#include "PlatformHandle_Win.h"

#include "YFramework.h"

#ifndef HID_USAGE_PAGE_GENERIC
  #define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
  #define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

namespace YEngine { namespace YFramework {

static_assert(sizeof(WinPlatformHandle) <= sizeof(PlatformHandle),
              "WinPlatformHandle must be smaller than PlatformHandle");

void InitializePlatformHandle(PlatformHandle& handle, const HWND hwnd) {
  WinPlatformHandle& winHandle = reinterpret_cast<WinPlatformHandle&>(handle);
  winHandle.mHwnd = hwnd;
}

LRESULT WINAPI WinMsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  switch (msg) {
    case WM_KEYDOWN:
      switch (wparam) {
      case VK_ESCAPE:
        YFramework::EndFramework();
        PostMessage( hwnd, WM_CLOSE, 0, 0);
        return 0;
      }

      // g_pInput->GetKeyboardInput()->ProcessKeyDown( wparam ); break;
      break;

    case WM_KEYUP:
      // g_pInput->GetKeyboardInput()->ProcessKeyUp( wparam ); break;
      break;

    case WM_INPUT:
      {
        UINT dwSize = 40;
        static BYTE lpb[40];
        
        GetRawInputData((HRAWINPUT)lparam, RID_INPUT, 
                lpb, &dwSize, sizeof(RAWINPUTHEADER));

        RAWINPUT* raw = (RAWINPUT*)lpb;
        
        if (raw->header.dwType == RIM_TYPEMOUSE) 
        {
          if ( raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE )
          {
            // g_pInput->GetMouseInput()->ProcessDeltaMouse( raw->data.mouse.lLastX, raw->data.mouse.lLastY );
          }
        } 
      }
      break;

    case WM_DESTROY:
      YFramework::EndFramework();
      PostQuitMessage(0);
      return 1;
  }

  return DefWindowProc(hwnd, msg, wparam, lparam);
}

/***********************
 * PlatformHandle Function Definitions for Windows
 ***********************/
void PlatformHandle::InitializePlatform() {
  WinPlatformHandle* winHandle = reinterpret_cast<WinPlatformHandle*>(mBuffer);

  RAWINPUTDEVICE Rid;
  Rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
  Rid.usUsage = HID_USAGE_GENERIC_MOUSE; 
  Rid.dwFlags = RIDEV_INPUTSINK;
  Rid.hwndTarget = winHandle->mHwnd;
  RegisterRawInputDevices(&Rid, 1, sizeof(Rid));
}

void PlatformHandle::ProcessPlatform() {
    // Process Window Message Pumps
    MSG msg;
    while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
}

}} // namespace YEngine { namespace YFramework {
