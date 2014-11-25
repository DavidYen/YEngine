#ifndef YCORE_YASSERT_H
#define YCORE_YASSERT_H

namespace YEngine { namespace YFramework {
  struct PlatformHandle;
}}

namespace YEngine { namespace YCore {

namespace YAssert{
  void Initialize(const YEngine::YFramework::PlatformHandle& platform_handle);
  void Terminate();

  void Assert(const char* file, uint32_t line, const char* message);
  void Warn(const char* file, uint32_t line, const char* message);
}

}} // namespace YEngine { namespace YCore {

#endif // YCORE_YASSERT_H
