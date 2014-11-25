#ifndef YCOMMON_YPLATFORM_PLATFORM_H
#define YCOMMON_YPLATFORM_PLATFORM_H

typedef void (*OnCloseFunc)();

namespace YCommon { namespace YPlatform {

struct PlatformHandle;

namespace Platform {
  void Init(const PlatformHandle& platform_handle,
            const OnCloseFunc on_close_func);
  void Release();

  void Update();
}

}} // namespace YCommon { namespace YPlatform {

#endif // YCOMMON_YPLATFORM_PLATFORM_H
