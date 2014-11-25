#ifndef YCOMMON_YUTILS_ASSERT_H
#define YCOMMON_YUTILS_ASSERT_H

namespace YCommon { namespace YPlatform {
  struct PlatformHandle;
}}

namespace YCommon { namespace YUtils {

namespace Assert{
  void Initialize(const YCommon::YPlatform::PlatformHandle& platform_handle);
  void Terminate();

  void Assert(const char* file, uint32_t line, const char* message);
  void Warn(const char* file, uint32_t line, const char* message);
}

}} // namespace YCommon { namespace YUtils {

#endif // YCOMMON_YUTILS_ASSERT_H
