#ifndef YCOMMON_PLATFORM_PLATFORM_H
#define YCOMMON_PLATFORM_PLATFORM_H

typedef void (*OnCloseFunc)();

namespace ycommon { namespace platform {

struct PlatformHandle;

namespace Platform {
  void Init(const PlatformHandle& platform_handle,
            const OnCloseFunc on_close_func);
  void Release();

  void Update();
}

}} // namespace ycommon { namespace platform {

#endif // YCOMMON_PLATFORM_PLATFORM_H
