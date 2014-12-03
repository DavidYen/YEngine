#ifndef YCOMMON_HEADERS_MACROS_H
#define YCOMMON_HEADERS_MACROS_H

#define IS_POWER_OF_2(num) (!(num & (num - 1)))

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

#endif // YCOMMON_HEADERS_MACROS_H
