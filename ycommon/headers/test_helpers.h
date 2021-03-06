#ifndef YCOMMON_HEADERS_TEST_HELPERS_H
#define YCOMMON_HEADERS_TEST_HELPERS_H

#include <gtest/gtest-spi.h>

#define TEST_FAILURE(BASE, TEST_NAME, FAILURE_STR) \
  void BASE ## _ ## TEST_NAME ## _FAILURE(); \
  TEST(BASE, TEST_NAME) { \
    EXPECT_FATAL_FAILURE(BASE ## _ ## TEST_NAME ## _FAILURE(), FAILURE_STR); \
  } \
  void BASE ## _ ## TEST_NAME ## _FAILURE()

#define TEST_FAILURE_F(BASE, TEST_NAME, FAILURE_STR) \
  void BASE ## _ ## TEST_NAME ## _FAILURE(); \
  TEST_F(BASE, TEST_NAME) { \
    EXPECT_FATAL_FAILURE(BASE ## _ ## TEST_NAME ## _FAILURE(), FAILURE_STR); \
  } \
  void BASE ## _ ## TEST_NAME ## _FAILURE()

#endif // YCOMMON_HEADERS_TEST_HELPERS_H
