#include "ycommon/utils/assert.h"

#include <gtest/gtest.h>

namespace testing { namespace internal {
  enum GTestColor {
    COLOR_DEFAULT,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW
  };
  void ColoredPrintf(GTestColor color, const char* fmt, ...);
}} // namespace testing { namespace internal {

namespace ycommon { namespace utils {

void Assert::Initialize(const platform::PlatformHandle&) {
}

void Assert::Terminate() {
}

void Assert::Assert(const char* file, uint32_t line, const char* message) {
  FAIL() << "[" << file << ":" << line << "] " << message;
}

void Assert::Warn(const char* file, uint32_t line, const char* message) {
  ::testing::internal::ColoredPrintf(
      ::testing::internal::COLOR_GREEN, "[          ] ");
  ::testing::internal::ColoredPrintf(
      ::testing::internal::COLOR_YELLOW, "[%s:%u] %s\n", file, line, message);
}

}} // namespace ycommon { namespace utils {
