
#include <stdint.h>
#include <YCommon/YUtils/YAssert.h>

#ifndef PLATINUM
# define YASSERT(cond, ...) \
  if (!(cond)) { \
    char buffer[512]; \
    snprintf(buffer, sizeof(buffer)-1, __VA_ARGS__); \
    YCommon::YUtils::YAssert::Assert(__FILE__, __LINE__, buffer); \
  }
# define YWARN(cond, ...) \
  if (!(cond)) { \
    char buffer[512]; \
    snprintf(buffer, sizeof(buffer)-1, __VA_ARGS__); \
    YCommon::YUtils::::YAssert::Warn(__FILE__, __LINE__, buffer); \
  }
#else
# define YASSERT(cond, ...)
#endif // PLATINUM

#ifdef DEBUG
# define YDEBUG_CHECK(cond, ...) YASSERT(cond, __VA_ARGS__)
#else
# define YDEBUG_CHECK(cond, ...)
#endif