#ifndef YCOMMON_HEADERS_MACROS_H
#define YCOMMON_HEADERS_MACROS_H

#include <YCommon/YUtils/Assert.h>

/***********
* Power of 2 functions
************/
#define IS_POWER_OF_2(num) (!((num) & ((num) - 1)))
#define ROUND_DOWN(num, alignment) ((num) & ~((alignment)-1))
#define ROUND_UP(num, alignment) ROUND_DOWN((num)+((alignment)-1), (alignment))

/***********
* See: http://cnicholson.net/2011/01/stupid-c-tricks-a-better-sizeof_array/
************/
namespace array_size_helper {
  template<typename T, size_t N>
  uint8_t (&SIZEOF_ARRAY_REQUIRES_ARRAY_ARGUMENT(T (&)[N]))[N];
}
#define ARRAY_SIZE(x) \
    sizeof(array_size_helper::SIZEOF_ARRAY_REQUIRES_ARRAY_ARGUMENT(x))

/***********
* ALIGN_FRONT(x) ALIGN_BACK(x) for each compiler.
************/
#ifdef _MSC_VER
  #define ALIGN_FRONT(x) __declspec(align(x))
  #define ALIGN_BACK(x)
#else
  #define ALIGN_FRONT(x)
  #define ALIGN_BACK(x) __attribute__ ((aligned (x)))
#endif

/*******
* Assert Macros
********/
#ifndef PLATINUM
# define YFATAL(...) \
  { \
    char buffer[512]; \
    snprintf(buffer, sizeof(buffer)-1, __VA_ARGS__); \
    YCommon::YUtils::Assert::Assert(__FILE__, __LINE__, buffer); \
  }
# define YASSERT(cond, ...) \
  if (!(cond)) { \
    char buffer[512]; \
    snprintf(buffer, sizeof(buffer)-1, __VA_ARGS__); \
    YCommon::YUtils::Assert::Assert(__FILE__, __LINE__, buffer); \
  }
# define YWARN(cond, ...) \
  if (!(cond)) { \
    char buffer[512]; \
    snprintf(buffer, sizeof(buffer)-1, __VA_ARGS__); \
    YCommon::YUtils::Assert::Warn(__FILE__, __LINE__, buffer); \
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


#endif // YCOMMON_HEADERS_MACROS_H
