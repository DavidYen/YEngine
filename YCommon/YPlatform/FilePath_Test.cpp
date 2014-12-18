#include <YCommon/Headers/stdincludes.h>
#include "FilePath.h"

#include <gtest/gtest.h>

#ifdef WIN
  #define PATH_SEP "\\"
#else
  #define PATH_SEP "/"
#endif

namespace YCommon { namespace YPlatform {

TEST(JoinPathsTest, BasicJoinTest) {
  char dest[4] = { '\0' };
  size_t dest_len = 0;

  ASSERT_TRUE(FilePath::JoinPaths("a", 1, "b", 1,
                                  dest, sizeof(dest), &dest_len));
  EXPECT_STREQ("a" PATH_SEP "b", dest);
  EXPECT_EQ(3, dest_len);
}

TEST(JoinPathsTest, JoinFailTest) {
  char dest[3] = { '\0' };
  size_t dest_len = 0;

  ASSERT_FALSE(FilePath::JoinPaths("a", 1, "b", 1,
                                   dest, sizeof(dest), &dest_len));
}

TEST(JoinPathsTest, JoinVariantPathTest) {
  char dest[64]  = { '\0' };
  size_t dest_len = 0;

  ASSERT_TRUE(FilePath::JoinPaths("a", 1, PATH_SEP "b", 2,
                                  dest, sizeof(dest), &dest_len));
  EXPECT_STREQ(PATH_SEP "b", dest);
  EXPECT_EQ(2, dest_len);

  ASSERT_TRUE(FilePath::JoinPaths(PATH_SEP "a", 2, "b", 1,
                                  dest, sizeof(dest), &dest_len));
  EXPECT_STREQ(PATH_SEP "a" PATH_SEP "b", dest);
  EXPECT_EQ(4, dest_len);

  ASSERT_TRUE(FilePath::JoinPaths("a" PATH_SEP, 2, "b", 1,
                                  dest, sizeof(dest), &dest_len));
  EXPECT_STREQ("a" PATH_SEP "b", dest);
  EXPECT_EQ(3, dest_len);
}

TEST(NormPathTest, BasicNormPathTest) {
  char dest[4]  = { '\0' };
  size_t dest_len = 0;

  const char test1[] = "a" PATH_SEP "b";
  ASSERT_TRUE(FilePath::NormPath(test1, sizeof(test1)-1,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ("a" PATH_SEP "b", dest);
  EXPECT_EQ(3, dest_len);
}

TEST(NormPathTest, FailedNormPathTest) {
  char dest[3]  = { '\0' };
  size_t dest_len = 0;

  const char test1[] = "a" PATH_SEP "b";
  ASSERT_FALSE(FilePath::NormPath(test1, sizeof(test1)-1,
                                  dest, sizeof(dest), &dest_len));
}

#ifdef WIN
TEST(NormPathTest, AltPathSepTest) {
  char dest[64]  = { '\0' };
  size_t dest_len = 0;

  ASSERT_TRUE(FilePath::NormPath("a\\b", 3,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ("a\\b", dest);
  EXPECT_EQ(3, dest_len);

  ASSERT_TRUE(FilePath::NormPath("a/b", 3,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ("a\\b", dest);
  EXPECT_EQ(3, dest_len);
}
#endif

TEST(NormPathTest, ParentDirTest) {
  char dest[64]  = { '\0' };
  size_t dest_len = 0;

  const char test1[] = "a" PATH_SEP "..";
  ASSERT_TRUE(FilePath::NormPath(test1, sizeof(test1)-1,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ("", dest);
  EXPECT_EQ(0, dest_len);

  const char test2[] = "a" PATH_SEP ".." PATH_SEP;
  ASSERT_TRUE(FilePath::NormPath(test2, sizeof(test2)-1,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ("", dest);
  EXPECT_EQ(0, dest_len);
}

TEST(NormPathTest, InvalidParentDirTest) {
  char dest[64]  = { '\0' };
  size_t dest_len = 0;

  const char test1[] = ".." PATH_SEP "a";
  ASSERT_TRUE(FilePath::NormPath(test1, sizeof(test1)-1,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ(test1, dest);
  EXPECT_EQ(strlen(test1), dest_len);

  const char test2[] = "a" PATH_SEP "..test";
  ASSERT_TRUE(FilePath::NormPath(test2, sizeof(test2)-1,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ(test2, dest);
  EXPECT_EQ(strlen(test2), dest_len);

  const char test3[] = "a..b";
  ASSERT_TRUE(FilePath::NormPath(test3, sizeof(test3)-1,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ(test3, dest);
  EXPECT_EQ(strlen(test3), dest_len);

  const char test4[] = ".." PATH_SEP ".." PATH_SEP "a";
  ASSERT_TRUE(FilePath::NormPath(test4, sizeof(test4)-1,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ(test4, dest);
  EXPECT_EQ(strlen(test4), dest_len);

  const char test5[] = ".." PATH_SEP "a" PATH_SEP ".." PATH_SEP "..";
  ASSERT_TRUE(FilePath::NormPath(test5, sizeof(test5)-1,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ(".." PATH_SEP "..", dest);
  EXPECT_EQ(strlen(".." PATH_SEP ".."), dest_len);
}

TEST(NormPathTest, CurrentDirTest) {
  char dest[64]  = { '\0' };
  size_t dest_len = 0;

  const char test1[] = "a" PATH_SEP "." PATH_SEP;
  ASSERT_TRUE(FilePath::NormPath(test1, sizeof(test1)-1,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ("a" PATH_SEP, dest);
  EXPECT_EQ(strlen("a" PATH_SEP), dest_len);

  const char test2[] = "." PATH_SEP "a";
  ASSERT_TRUE(FilePath::NormPath(test2, sizeof(test2)-1,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ("a", dest);
  EXPECT_EQ(strlen("a"), dest_len);

  const char test3[] = "." PATH_SEP "a" PATH_SEP;
  ASSERT_TRUE(FilePath::NormPath(test3, sizeof(test3)-1,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ("a" PATH_SEP, dest);
  EXPECT_EQ(strlen("a" PATH_SEP), dest_len);
}

TEST(NormPathTest, InvalidCurrentDirTest) {
  char dest[64]  = { '\0' };
  size_t dest_len = 0;

  const char test1[] = "a" PATH_SEP ".test";
  ASSERT_TRUE(FilePath::NormPath(test1, sizeof(test1)-1,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ("a" PATH_SEP ".test", dest);
  EXPECT_EQ(strlen("a" PATH_SEP ".test"), dest_len);

  const char test2[] = "a.b";
  ASSERT_TRUE(FilePath::NormPath(test2, sizeof(test2)-1,
                                 dest, sizeof(dest), &dest_len));
  EXPECT_STREQ("a.b", dest);
  EXPECT_EQ(strlen("a.b"), dest_len);
}

TEST(AbsPathTest, BasicAbsPathTest) {
  char dest[4]  = { '\0' };
  size_t dest_len = 0;

  ASSERT_TRUE(FilePath::AbsPath("b", strlen("b"),
                                dest, sizeof(dest), &dest_len,
                                "a", strlen("a")));
  EXPECT_STREQ("a" PATH_SEP "b", dest);
  EXPECT_EQ(strlen("a" PATH_SEP "b"), dest_len);
}

TEST(AbsPathTest, FailedAbsPathTest) {
  char dest[3]  = { '\0' };
  size_t dest_len = 0;

  ASSERT_FALSE(FilePath::AbsPath("b", strlen("b"),
                                 dest, sizeof(dest), &dest_len,
                                 "a", strlen("a")));
}

TEST(AbsPathTest, DoubleAbsPathTest) {
  char dest[32]  = { '\0' };
  size_t dest_len = 0;

  const char* path = "/a";
  const char* work_dir = "/b";
  ASSERT_TRUE(FilePath::AbsPath(path, strlen(path),
                                dest, sizeof(dest), &dest_len,
                                work_dir, strlen(work_dir)));

  const char* expected = PATH_SEP "a";
  EXPECT_STREQ(expected, dest);
  EXPECT_EQ(strlen(expected), dest_len);
}

#ifdef WIN
TEST(AbsPathTest, AbsWinPathTest) {
  char dest[32]  = { '\0' };
  size_t dest_len = 0;

  const char* path = "a";
  const char* work_dir = "c:\\b";
  ASSERT_TRUE(FilePath::AbsPath(path, strlen(path),
                                dest, sizeof(dest), &dest_len,
                                work_dir, strlen(work_dir)));

  const char* expected = "c:\\b\\a";
  EXPECT_STREQ(expected, dest);
  EXPECT_EQ(strlen(expected), dest_len);
}

TEST(AbsPathTest, DoubleAbsWinPathTest) {
  char dest[32]  = { '\0' };
  size_t dest_len = 0;

  const char* path = "c:\\a";
  const char* work_dir = "c:\\b";
  ASSERT_TRUE(FilePath::AbsPath(path, strlen(path),
                                dest, sizeof(dest), &dest_len,
                                work_dir, strlen(work_dir)));

  const char* expected = "c:\\a";
  EXPECT_STREQ(expected, dest);
  EXPECT_EQ(strlen(expected), dest_len);
}
#endif

TEST(AbsPathTest, RelativePathTest) {
  char dest[4]  = { '\0' };
  size_t dest_len = 0;

  ASSERT_TRUE(FilePath::AbsPath("..", strlen(".."),
                                dest, sizeof(dest), &dest_len,
                                "a", strlen("a")));
  EXPECT_STREQ("", dest);
  EXPECT_EQ(strlen(""), dest_len);
}

TEST(RelPathTest, BasicTest) {
  char dest[2]  = { '\0' };
  size_t dest_len = 0;

  const char* path = "abc" PATH_SEP "b";
  const char* dir = "abc";
  ASSERT_TRUE(FilePath::RelPath(path, strlen(path),
                                dir, strlen(dir),
                                dest, sizeof(dest), &dest_len));
  EXPECT_STREQ("b", dest);
  EXPECT_EQ(strlen("b"), dest_len);
}

TEST(RelPathTest, PathSepEndTest) {
  char dest2[3] = { '\0' };
  size_t dest2_len = 0;
  const char* path2 = "abc" PATH_SEP "b" PATH_SEP;
  const char* dir2 = "abc" PATH_SEP;
  ASSERT_TRUE(FilePath::RelPath(path2, strlen(path2),
                                dir2, strlen(dir2),
                                dest2, sizeof(dest2), &dest2_len));
  EXPECT_STREQ("b" PATH_SEP, dest2);
  EXPECT_EQ(strlen("b" PATH_SEP), dest2_len);
}

TEST(RelPathTest, FailedRelTest) {
  char dest[3]  = { '\0' };
  size_t dest_len = 0;

  const char* path = "a" PATH_SEP "abc";
  const char* dir = "a";
  ASSERT_FALSE(FilePath::RelPath(path, strlen(path),
                                 dir, strlen(dir),
                                 dest, sizeof(dest), &dest_len));
}

TEST(RelPathTest, CompleteMatchTest) {
  char dest[32]  = { '\0' };
  size_t dest_len = 0;

  const char* path = "abc" PATH_SEP "def";
  const char* dir = "abc" PATH_SEP "def";
  ASSERT_TRUE(FilePath::RelPath(path, strlen(path),
                                dir, strlen(dir),
                                dest, sizeof(dest), &dest_len));

  const char* expected = "";
  EXPECT_STREQ(expected, dest);
  EXPECT_EQ(strlen(expected), dest_len);

  const char* dir2 = "abc" PATH_SEP "def" PATH_SEP;
  ASSERT_TRUE(FilePath::RelPath(path, strlen(path),
                                dir2, strlen(dir2),
                                dest, sizeof(dest), &dest_len));

  EXPECT_STREQ(expected, dest);
  EXPECT_EQ(strlen(expected), dest_len);

  const char* path2 = "abc" PATH_SEP "def" PATH_SEP;
  ASSERT_TRUE(FilePath::RelPath(path2, strlen(path2),
                                dir, strlen(dir),
                                dest, sizeof(dest), &dest_len));

  EXPECT_STREQ(expected, dest);
  EXPECT_EQ(strlen(expected), dest_len);

  ASSERT_TRUE(FilePath::RelPath(path2, strlen(path2),
                                dir2, strlen(dir2),
                                dest, sizeof(dest), &dest_len));

  EXPECT_STREQ(expected, dest);
  EXPECT_EQ(strlen(expected), dest_len);
}

TEST(RelPathTest, NoMatchTest) {
  char dest[32]  = { '\0' };
  size_t dest_len = 0;

  const char* path = "abc" PATH_SEP "def";
  const char* dir = "abcd" PATH_SEP "ghi";
  ASSERT_TRUE(FilePath::RelPath(path, strlen(path),
                                dir, strlen(dir),
                                dest, sizeof(dest), &dest_len));

  const char* expected = ".." PATH_SEP ".." PATH_SEP "abc" PATH_SEP "def";
  EXPECT_STREQ(expected, dest);
  EXPECT_EQ(strlen(expected), dest_len);
}

TEST(RelPathTest, PartialMatchTest) {
  char dest[32]  = { '\0' };
  size_t dest_len = 0;

  const char* path = "abc" PATH_SEP "def";
  const char* dir = "abc" PATH_SEP "jkl";
  ASSERT_TRUE(FilePath::RelPath(path, strlen(path),
                                dir, strlen(dir),
                                dest, sizeof(dest), &dest_len));

  const char* expected = ".." PATH_SEP "def";
  EXPECT_STREQ(expected, dest);
  EXPECT_EQ(strlen(expected), dest_len);
}

TEST(DirPathTest, BasicTest) {
  char dest[4] = { '\0' };
  size_t dest_len = 0;

  const char* path = "abc" PATH_SEP "def";
  ASSERT_TRUE(FilePath::DirPath(path, strlen(path),
                                dest, sizeof(dest), &dest_len));

  const char* expected = "abc";
  EXPECT_STREQ(expected, dest);
  EXPECT_EQ(strlen(expected), dest_len);
}

TEST(DirPathTest, BasicFailTest) {
  char dest[3] = { '\0' };
  size_t dest_len = 0;

  const char* path = "abc" PATH_SEP "def";
  ASSERT_FALSE(FilePath::DirPath(path, strlen(path),
                                 dest, sizeof(dest), &dest_len));
}

TEST(DirPathTest, RootTest) {
  char dest[4] = { '\0' };
  size_t dest_len = 0;

  const char* path = PATH_SEP "abc";
  ASSERT_TRUE(FilePath::DirPath(path, strlen(path),
                                dest, sizeof(dest), &dest_len));

  const char* expected = PATH_SEP;
  EXPECT_STREQ(expected, dest);
  EXPECT_EQ(strlen(expected), dest_len);
}

}} // namespace YCommon { namespace YPlatform {
