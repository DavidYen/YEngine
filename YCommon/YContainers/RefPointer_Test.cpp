#include <YCommon/Headers/stdincludes.h>
#include "RefPointer.h"

#include <gtest/gtest.h>

#include <YCommon/Headers/TestHelpers.h>

namespace YCommon { namespace YContainers {

TEST(RefPointerTest, EmptyConstructorTest) {
  RefPointer empty_ref_pointer;
}

TEST(RefPointerTest, BasicConstructorTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(basic_ref_pointer, &basic);
}

TEST(RefPointerTest, BasicAssignmentTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer;
  EXPECT_NE(basic_ref_pointer, &basic);
  basic_ref_pointer = &basic;
  EXPECT_EQ(basic_ref_pointer, &basic);
}

TEST(RefPointerTest, ReadRefTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadRef());
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadRef());
  basic_ref_pointer.ReleaseReadRef();
  basic_ref_pointer.ReleaseReadRef();
}

TEST_FAILURE(RefPointerTest, ReadRefUnreleasedFail,
             "Read references not zero") {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadRef());
}

TEST_FAILURE(RefPointerTest, ReadRefWithWriteFail,
             "Write references not zero") {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(&basic, basic_ref_pointer.GetWriteRef());
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadRef());

  basic_ref_pointer.ReleaseReadRef();
  basic_ref_pointer.ReleaseWriteRef();
}

TEST_FAILURE(RefPointerTest, ReadRefWithReadWriteFail,
             "Read/Write references not zero") {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadWriteRef());
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadRef());

  basic_ref_pointer.ReleaseReadRef();
  basic_ref_pointer.ReleaseReadWriteRef();
}

TEST(RefPointerTest, WriteRefTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(&basic, basic_ref_pointer.GetWriteRef());
  EXPECT_EQ(&basic, basic_ref_pointer.GetWriteRef());
  basic_ref_pointer.ReleaseWriteRef();
  basic_ref_pointer.ReleaseWriteRef();
}

TEST_FAILURE(RefPointerTest, WriteRefUnreleasedFail,
             "Write references not zero") {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(&basic, basic_ref_pointer.GetWriteRef());
}

TEST_FAILURE(RefPointerTest, WriteRefWithReadFail,
             "Read references not zero") {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadRef());
  EXPECT_EQ(&basic, basic_ref_pointer.GetWriteRef());

  basic_ref_pointer.ReleaseWriteRef();
  basic_ref_pointer.ReleaseReadRef();
}

TEST_FAILURE(RefPointerTest, WriteRefWithReadWriteFail,
             "Read/Write references not zero") {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadWriteRef());
  EXPECT_EQ(&basic, basic_ref_pointer.GetWriteRef());

  basic_ref_pointer.ReleaseWriteRef();
  basic_ref_pointer.ReleaseReadWriteRef();
}

TEST(RefPointerTest, ReadWriteRefTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadWriteRef());
  basic_ref_pointer.ReleaseReadWriteRef();
}

TEST_FAILURE(RefPointerTest, ReadWriteRefUnreleasedFail,
             "Write references not zero") {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadWriteRef());
}

TEST_FAILURE(RefPointerTest, ReadWriteRefWithReadFail,
             "Read references not zero") {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadRef());
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadWriteRef());

  basic_ref_pointer.ReleaseReadWriteRef();
  basic_ref_pointer.ReleaseReadRef();
}

TEST_FAILURE(RefPointerTest, ReadWriteRefWithWriteFail,
             "Write references not zero") {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(&basic, basic_ref_pointer.GetWriteRef());
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadWriteRef());

  basic_ref_pointer.ReleaseWriteRef();
  basic_ref_pointer.ReleaseReadWriteRef();
  }

TEST_FAILURE(RefPointerTest, ReadWriteRefWithReadWriteFail,
             "Read/Write references not zero") {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadWriteRef());
  EXPECT_EQ(&basic, basic_ref_pointer.GetReadWriteRef());

  basic_ref_pointer.ReleaseReadWriteRef();
  basic_ref_pointer.ReleaseReadWriteRef();
}

}} // namespace YCommon { namespace YContainers {
