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
  ReadRefData ref1 = basic_ref_pointer.GetReadRef();
  ReadRefData ref2 = basic_ref_pointer.GetReadRef();
  EXPECT_EQ(ref1, &basic);
  EXPECT_EQ(ref2, &basic);
}

TEST_FAILURE(RefPointerTest, ReadRefUnreleasedFail,
             "Read references not zero") {
  RefPointer basic_ref_pointer;
  ReadRefData read_ref = basic_ref_pointer.GetReadRef();
  basic_ref_pointer.Reset();
}

TEST_FAILURE(RefPointerTest, ReadRefWithWriteFail,
             "Write references not zero") {
  RefPointer basic_ref_pointer;
  WriteRefData write_ref = basic_ref_pointer.GetWriteRef();
  ReadRefData read_ref = basic_ref_pointer.GetReadRef();
}

TEST_FAILURE(RefPointerTest, ReadRefWithReadWriteFail,
             "Read/Write references not zero") {
  RefPointer basic_ref_pointer;
  ReadWriteRefData read_write_ref = basic_ref_pointer.GetReadWriteRef();
  ReadRefData read_ref = basic_ref_pointer.GetReadRef();
}

TEST(RefPointerTest, WriteRefTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  WriteRefData ref1 = basic_ref_pointer.GetWriteRef();
  WriteRefData ref2 = basic_ref_pointer.GetWriteRef();
  EXPECT_EQ(ref1, &basic);
  EXPECT_EQ(ref2, &basic);
}

TEST_FAILURE(RefPointerTest, WriteRefUnreleasedFail,
             "Write references not zero") {
  RefPointer basic_ref_pointer;
  WriteRefData write_ref = basic_ref_pointer.GetWriteRef();
  basic_ref_pointer.Reset();
}

TEST_FAILURE(RefPointerTest, WriteRefWithReadFail,
             "Read references not zero") {
  RefPointer basic_ref_pointer;
  ReadRefData read_ref = basic_ref_pointer.GetReadRef();
  WriteRefData write_ref = basic_ref_pointer.GetWriteRef();
}

TEST_FAILURE(RefPointerTest, WriteRefWithReadWriteFail,
             "Read/Write references not zero") {
  RefPointer basic_ref_pointer;
  ReadWriteRefData read_write_ref = basic_ref_pointer.GetReadWriteRef();
  WriteRefData write_ref = basic_ref_pointer.GetWriteRef();
}

TEST(RefPointerTest, ReadWriteRefTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  ReadWriteRefData ref = basic_ref_pointer.GetReadWriteRef();
  EXPECT_EQ(ref, &basic);
}

TEST_FAILURE(RefPointerTest, ReadWriteRefUnreleasedFail,
             "Write references not zero") {
  RefPointer basic_ref_pointer;
  ReadWriteRefData read_write_ref = basic_ref_pointer.GetReadWriteRef();
  basic_ref_pointer.Reset();
}

TEST_FAILURE(RefPointerTest, ReadWriteRefWithReadFail,
             "Read references not zero") {
  RefPointer basic_ref_pointer;
  ReadRefData read_ref = basic_ref_pointer.GetReadRef();
  ReadWriteRefData read_write_ref = basic_ref_pointer.GetReadWriteRef();
}

TEST_FAILURE(RefPointerTest, ReadWriteRefWithWriteFail,
             "Write references not zero") {
  RefPointer basic_ref_pointer;
  WriteRefData write_ref = basic_ref_pointer.GetWriteRef();
  ReadWriteRefData read_write_ref = basic_ref_pointer.GetReadWriteRef();
}

TEST_FAILURE(RefPointerTest, ReadWriteRefWithReadWriteFail,
             "Read/Write references not zero") {
  RefPointer basic_ref_pointer;
  ReadWriteRefData read_write_ref1 = basic_ref_pointer.GetReadWriteRef();
  ReadWriteRefData read_write_ref2 = basic_ref_pointer.GetReadWriteRef();
}

}} // namespace YCommon { namespace YContainers {
