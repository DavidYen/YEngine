#include <YCommon/Headers/stdincludes.h>
#include "Assert.h"

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

namespace YCommon { namespace YUtils {

void Assert::Initialize(const YCommon::YPlatform::PlatformHandle&) {
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
      ::testing::internal::COLOR_YELLOW, "[%s:%u] %s", file, line, message);
}

}} // namespace YCommon { namespace YUtils {
