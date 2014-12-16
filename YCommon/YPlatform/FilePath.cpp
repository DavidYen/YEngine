#include <YCommon/Headers/stdincludes.h>
#include "FilePath.h"

#include <string>

#ifdef WIN
  #define IS_PATH_SEP(PATH_CHAR) (PATH_CHAR == '/' || PATH_CHAR == '\\')
  #define PATH_SEP '\\'
#else
  #define IS_PATH_SEP(PATH_CHAR) (PATH_CHAR == '/')
  #define PATH_SEP '/'
#endif

namespace YCommon { namespace YPlatform {

// Join Paths
bool FilePath::JoinPaths(const char* dir, size_t dir_len,
                         const char* path, size_t path_len,
                         char* dest, size_t dest_size, size_t* dest_len) {
  char* dest_iter = dest;
  char* dest_end = dest + dest_size;

  if (IS_PATH_SEP(*path)) {
    if (dest_size <= path_len)
      return false;
    memcpy(dest_iter, path, path_len + 1);
    dest_iter += path_len;
  } else {
    if (dest_size <= dir_len)
      return false;

    if (dest_iter + dir_len > dest_end)
      return false;
    memcpy(dest_iter, dir, dir_len);
    dest_iter += dir_len;

    if (dir_len > 0 && !IS_PATH_SEP(dir[dir_len-1])) {
      if (dest_iter == dest_end)
        return false;
      *dest_iter++ = PATH_SEP;
    }

    if (dest_iter + (path_len + 1) > dest_end)
      return false;
    memcpy(dest_iter, path, (path_len + 1));
    dest_iter += path_len;
  }

  if (dest_len)
    *dest_len = (dest_iter - dest);
  return true;
}

// Normalize Path
bool FilePath::NormPath(const char* path, size_t path_len,
                        char* dest, size_t dest_size, size_t* dest_len) {
  (void) path_len;

  char* dest_iter = dest;
  char* dest_end = dest + dest_size - 1;

  enum ParseMode {
    kParseMode_Begin,
    kParseMode_PathStart,
    kParseMode_Path
  } parse_mode = kParseMode_Begin;

  for (const char* path_iter = path; *path_iter != '\0'; ++path_iter) {
    if (dest_iter == dest_end)
      return false;

    const char path_char = *path_iter;

    if (IS_PATH_SEP(path_char)) {
      // Only add sep if not already start of a directory.
      if (parse_mode != kParseMode_PathStart)
        *dest_iter++ = PATH_SEP;
      parse_mode = kParseMode_PathStart;
    } else {
      // Process special rules if beginning of a directory.
      if (parse_mode != kParseMode_Path) {
        if (path_char == '.') {
          const char next_path_char = *(path_iter + 1);

          // Current Directory?
          if (IS_PATH_SEP(next_path_char)) {
            ++path_iter;
            continue;
          }

          // Parent Directory?
          if (parse_mode == kParseMode_PathStart &&
              next_path_char == '.') {
            const char next_next_path_char = *(path_iter + 2);
            if (IS_PATH_SEP(next_next_path_char) ||
                next_next_path_char == '\0') {
              // Go to parent directory
              char* prev_dir = dest_iter - 2;
              for (; prev_dir != dest; --prev_dir) {
                if (*prev_dir == PATH_SEP) {
                  ++prev_dir;
                  break;
                }
              }

              // Check for "..\" here.
              if (prev_dir[0] != '.' ||
                  prev_dir[1] != '.' ||
                  prev_dir[2] != PATH_SEP) {
                if (prev_dir == dest) {
                  dest_iter = dest;
                  parse_mode = kParseMode_Begin;
                } else {
                  dest_iter = prev_dir;
                }
                path_iter += 2;
                continue;
              }
            }
          }
        }
      }
      *dest_iter++ = path_char;
      parse_mode = kParseMode_Path;
    }
  }

  if (dest_len)
    *dest_len = (dest_iter - dest);
  *dest_iter = '\0';
  return true;
}

// Absolute Path
bool FilePath::AbsPath(const char* path, size_t path_len,
                       char* dest, size_t dest_size, size_t* dest_len,
                       const char* working_dir, size_t working_dir_len) {
  char temp_work_dir[256];
  size_t temp_work_dir_len = 0;
  if (working_dir == NULL || working_dir_len == 0) {
    if (!GetCurrentWorkingDirectory(temp_work_dir, sizeof(temp_work_dir),
                                    &temp_work_dir_len)) {
      return false;
    }
    working_dir = temp_work_dir;
    working_dir_len = temp_work_dir_len;
  }

  char joined_dir[256];
  size_t joined_dir_len = 0;
  if (!JoinPaths(working_dir, working_dir_len,
                 path, path_len,
                 joined_dir, sizeof(joined_dir), &joined_dir_len)) {
    return false;
  }

  return NormPath(joined_dir, joined_dir_len, dest, dest_size, dest_len);
}

#include <stdio.h>

// Relative Path
bool FilePath::RelPath(const char* path, size_t path_len,
                       const char* dir, size_t dir_len,
                       char* dest, size_t dest_size, size_t* dest_len) {
  char normalized_path[256];
  size_t normalized_path_len = 0;
  if (!NormPath(path, path_len, normalized_path, sizeof(normalized_path),
                &normalized_path_len)) {
    return false;
  }

  char normalized_dir[256];
  size_t normalized_dir_len = 0;
  if (!NormPath(dir, dir_len, normalized_dir, sizeof(normalized_dir),
                &normalized_dir_len)) {
    return false;
  }

  const char* path_iter = normalized_path;
  const char* dir_iter = normalized_dir;

  char* dest_iter = dest;
  char* dest_end = dest + dest_size - 1;

  // Skip matching directories
  {
    const char* try_path_iter = normalized_path;
    const char* try_dir_iter = normalized_dir;
    while (*try_path_iter == *try_dir_iter) {
      if (IS_PATH_SEP(*try_path_iter)) {
        // Full path detected, can skip up to here
        path_iter = try_path_iter + 1;
        dir_iter = try_dir_iter + 1;
      } else if (*try_path_iter == '\0') {
        // End of string detected
        path_iter = try_path_iter;
        dir_iter = try_dir_iter;
        break;
      }
      ++try_path_iter;
      ++try_dir_iter;
    }

    // If we got to the end of one of the strings, we must also check the other
    // for the path separator.
    if (*try_dir_iter == '\0') {
      if (IS_PATH_SEP(*try_path_iter)) {
        dir_iter = try_dir_iter;
        path_iter = try_path_iter + 1;
      }
    } else if (*try_path_iter == '\0') {
      if (IS_PATH_SEP(*try_dir_iter)) {
        path_iter = try_path_iter;
        dir_iter = try_dir_iter + 1;
      }
    }
  }

  if (*dir_iter != '\0') {
    // Add ".." for each directory left over in the normalized_dir.
    for (; *dir_iter != '\0'; ++dir_iter) {
      if (IS_PATH_SEP(*dir_iter)) {
        if (dest_iter + 3 >= dest_end) {
          return false;
        }
        dest_iter[0] = '.';
        dest_iter[1] = '.';
        dest_iter[2] = PATH_SEP;
        dest_iter += 3;
      }
    }

    // Add an extra one if the directory path didn't end with PATH_SEP.
    if (normalized_dir_len > 0 &&
        normalized_dir[normalized_dir_len-1] != PATH_SEP) {
      if (dest_iter + 3 >= dest_end) {
        return false;
      }
      dest_iter[0] = '.';
      dest_iter[1] = '.';
      dest_iter[2] = PATH_SEP;
      dest_iter += 3;
    }
  }

  // Copy the rest of the path.
  const size_t path_left = (normalized_path + normalized_path_len) - path_iter;
  if (dest_iter + path_left > dest_end) {
    return false;
  }
  memcpy(dest_iter, path_iter, path_left);
  dest_iter += path_left;

  if (dest_len) {
    *dest_len = dest_iter - dest;
  }

  *dest_iter = '\0';
  return true;
}

}} // namespace YCommon { namespace YPlatform {
