#ifndef YCOMMON_UTILS_ASSERT_H
#define YCOMMON_UTILS_ASSERT_H

#include <cstdio>
#include <stdint.h>

namespace ycommon { namespace platform {
  struct PlatformHandle;
}}

namespace ycommon { namespace utils {

namespace Assert {
  void Initialize(const platform::PlatformHandle& platform_handle);
  void Terminate();

  void Assert(const char* file, uint32_t line, const char* message);
  void Warn(const char* file, uint32_t line, const char* message);
} // namespace assert {

}} // namespace ycommon { namespace utils {

/*******
* Assert Macros
********/
#ifndef PLATINUM
# define YFATAL(...) \
  { \
    char __buffer[512]; \
    snprintf(__buffer, sizeof(__buffer)-1, __VA_ARGS__); \
    ycommon::utils::Assert::Assert(__FILE__, __LINE__, __buffer); \
  }
# define YASSERT(cond, ...) \
  if (!(cond)) { \
    char __buffer[512]; \
    snprintf(__buffer, sizeof(__buffer)-1, __VA_ARGS__); \
    ycommon::utils::Assert::Assert(__FILE__, __LINE__, __buffer); \
  }
# define YWARN(cond, ...) \
  if (!(cond)) { \
    char __buffer[512]; \
    snprintf(__buffer, sizeof(__buffer)-1, __VA_ARGS__); \
    ycommon::utils::Assert::Warn(__FILE__, __LINE__, __buffer); \
  }
#else
# define YFATAL(...)
# define YASSERT(cond, ...) do { static_cast<void>(cond); } while(0)
# define YWARN(cond, ...) do { static_cast<void>(cond); } while(0)
#endif // PLATINUM

#ifdef DEBUG
# define YDEBUG_CHECK(cond, ...) YASSERT(cond, __VA_ARGS__)
#else
# define YDEBUG_CHECK(cond, ...) do { static_cast<void>(cond); } while(0)
#endif

#endif // YCOMMON_UTILS_ASSERT_H
