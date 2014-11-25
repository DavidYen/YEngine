#ifndef YCOMMON_YPLATFORM_PLATFORM_WIN_H
#define YCOMMON_YPLATFORM_PLATFORM_WIN_H

#include <Windows.h>

namespace YCommon { namespace YPlatform {

LRESULT WINAPI WinMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lparam);

}} // namespace YCommon { namespace YPlatform {

#endif // YCOMMON_YPLATFORM_PLATFORM_WIN_H
