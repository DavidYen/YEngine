#ifndef YFRAMEWORK_PLATFORMHANDLE_H
#define YFRAMEWORK_PLATFORMHANDLE_H

namespace YEngine { namespace YFramework {

struct PlatformHandle {
  void InitializePlatform();
  void ProcessPlatform();

  char mBuffer[32];
};

}} // namespace YEngine { namespace YFramework {


#endif // YFRAMEWORK_PLATFORMHANDLE_H
