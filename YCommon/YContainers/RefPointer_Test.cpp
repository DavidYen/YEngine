#include <YCommon/Headers/stdincludes.h>
#include "RefPointer.h"

#include <utility>
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

TEST(RefPointerTest, BasicReadCounterTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(0u, basic_ref_pointer.ReadRefCount());
  ReadRefData read_ref = basic_ref_pointer.GetReadRef();
  EXPECT_EQ(1u, basic_ref_pointer.ReadRefCount());
}

TEST(RefPointerTest, BasicWriteCounterTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(0u, basic_ref_pointer.WriteRefCount());
  WriteRefData write_ref = basic_ref_pointer.GetWriteRef();
  EXPECT_EQ(1u, basic_ref_pointer.WriteRefCount());
}

TEST(RefPointerTest, BasicReadWriteCounterTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  EXPECT_EQ(0u, basic_ref_pointer.ReadWriteRefCount());
  ReadWriteRefData read_write_ref = basic_ref_pointer.GetReadWriteRef();
  EXPECT_EQ(1u, basic_ref_pointer.ReadWriteRefCount());
}

TEST(RefPointerTest, BasicAssignmentTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer;
  EXPECT_NE(basic_ref_pointer, &basic);
  basic_ref_pointer = &basic;
  EXPECT_EQ(basic_ref_pointer, &basic);
}

TEST(RefPointerTest, BasicMoveTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer1(&basic);
  RefPointer basic_ref_pointer2;

  EXPECT_EQ(basic_ref_pointer1, &basic);
  EXPECT_EQ(basic_ref_pointer2, nullptr);

  basic_ref_pointer2 = std::move(basic_ref_pointer1);
}

TEST(RefPointerTest, BasicCopyConstructorTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer1(&basic);

  ReadRefData read_ref = basic_ref_pointer1.GetReadRef();

  RefPointer basic_ref_pointer2(basic_ref_pointer1);

  EXPECT_EQ(basic_ref_pointer1, &basic);
  EXPECT_EQ(basic_ref_pointer2, &basic);

  // first one should still have references.
  EXPECT_EQ(1u, basic_ref_pointer1.ReadRefCount());

  // Copied should have no references yet 
  EXPECT_EQ(0u, basic_ref_pointer2.ReadRefCount());
}

TEST(RefPointerTest, ReadRefTest) {
  uint8_t basic = 1;
  RefPointer basic_ref_pointer(&basic);
  ReadRefData ref1 = basic_ref_pointer.GetReadRef();
  ReadRefData ref2 = basic_ref_pointer.GetReadRef();
  EXPECT_EQ(ref1, &basic);
  EXPECT_EQ(ref2, &basic);
}

TEST(RefPointerTest, ReadRefCopyTest) {
  RefPointer basic_ref_pointer;
  ReadRefData read_ref = basic_ref_pointer.GetReadRef();
  ReadRefData read_ref2(read_ref);
}

TEST(RefPointerTest, ReadRefAssignmentTest) {
  RefPointer basic_ref_pointer1;
  ReadRefData read_ref1 = basic_ref_pointer1.GetReadRef();

  RefPointer basic_ref_pointer2;
  ReadRefData read_ref2 = basic_ref_pointer2.GetReadRef();

  read_ref2 = read_ref1;
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

TEST(RefPointerTest, WriteRefCopyTest) {
  RefPointer basic_ref_pointer;
  WriteRefData write_ref = basic_ref_pointer.GetWriteRef();
  WriteRefData write_ref2(write_ref);
}

TEST(RefPointerTest, WriteRefAssignmentTest) {
  RefPointer basic_ref_pointer1;
  WriteRefData write_ref1 = basic_ref_pointer1.GetWriteRef();

  RefPointer basic_ref_pointer2;
  WriteRefData write_ref2 = basic_ref_pointer2.GetWriteRef();

  write_ref2 = write_ref1;
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

TEST(RefPointerTest, ReadWriteRefCopyTest) {
  RefPointer basic_ref_pointer;
  ReadWriteRefData read_write_ref = basic_ref_pointer.GetReadWriteRef();
  ReadWriteRefData read_write_ref2(read_write_ref);
}

TEST(RefPointerTest, ReadWriteRefAssignmentTest) {
  RefPointer basic_ref_pointer1;
  ReadWriteRefData read_write_ref1 = basic_ref_pointer1.GetReadWriteRef();

  RefPointer basic_ref_pointer2;
  ReadWriteRefData read_write_ref2 = basic_ref_pointer2.GetReadWriteRef();

  read_write_ref2 = read_write_ref1;
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
