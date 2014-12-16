#include <YCommon/Headers/stdincludes.h>
#include "FilePath.h"

#include <Windows.h>

namespace YCommon { namespace YPlatform {

bool FilePath::GetCurrentWorkingDirectory(char* dir, size_t dir_size,
                                          size_t* dest_len) {
  const DWORD ret = GetCurrentDirectory(static_cast<DWORD>(dir_size), dir);
  if (ret == 0 || ret == dir_size)
    return false;

  if (dest_len)
    *dest_len = static_cast<size_t>(ret);
  return true;
}

bool FilePath::IsFile(const char* filepath) {
  const DWORD attrib = GetFileAttributes(filepath);
  return (attrib != INVALID_FILE_ATTRIBUTES) &&
         ((attrib & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool FilePath::IsDir(const char* dirpath) {
  const DWORD attrib = GetFileAttributes(dirpath);
  return (attrib != INVALID_FILE_ATTRIBUTES) &&
         (attrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool FilePath::Exists(const char* path) {
  return GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES;
}

}} // namespace YCommon { namespace YPlatform {