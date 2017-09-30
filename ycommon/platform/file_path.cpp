#include "ycommon/platform/file_path.h"

#include <stdint.h>
#include <string>

#ifdef WIN
  #define IS_PATH_SEP(PATH_CHAR) (PATH_CHAR == '/' || PATH_CHAR == '\\')
  #define PATH_SEP '\\'
#else
  #define IS_PATH_SEP(PATH_CHAR) (PATH_CHAR == '/')
  #define PATH_SEP '/'
#endif

namespace ycommon { namespace platform {

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
// This function strips multiple path separators (///, \\\) and 
// interprets ./, ../
bool FilePath::NormPath(const char* path, size_t path_len,
                        char* dest, size_t dest_size, size_t* dest_len) {
  static_cast<void>(path_len);

  char* dest_iter = dest;
  char* dest_end = dest + dest_size - 1;

  const char* path_iter = path;

  // First check if this is an absolute path. Absolute path is a special case
  // where it begins with a path separator.
  if (IS_PATH_SEP(*path_iter)) {
    *dest_iter++ = PATH_SEP;

    ++path_iter;
    while (IS_PATH_SEP(*path_iter)) {
      ++path_iter;
    }
  }

  while (*path_iter != '\0') {
    // Check for possible relative directory references.
    if (*path_iter == '.') {
      // Current dir must be in the form of "./", then collapse all path seps.
      if (IS_PATH_SEP(path_iter[1]) || path_iter[1] == '\0') {
        ++path_iter;
        while (IS_PATH_SEP(*path_iter)) {
          ++path_iter;
        }
        continue;
      }

      // Parent dir must be in the form of "../", then collapse all path seps.
      if (path_iter[1] == '.' &&
          (IS_PATH_SEP(path_iter[2]) || path_iter[2] == '\0')) {
        // dest_iter must not be at the first directory, otherwise we can
        // move up. The first directory is either going to be "" or "/", so
        // checking if the current length is greater than 2 should be enough.
        if (dest_iter - dest >= 2) {
          char* prev_dir = dest_iter - 2;
          while (prev_dir != dest && !IS_PATH_SEP(*prev_dir)) {
            --prev_dir;
          }

          // We are at the previous path separator, move prev_dir to the directory name itself.
          if (prev_dir != dest)
            ++prev_dir;

          // Check if prev_dir is itself a parent directory, then we cannot
          // remove it.
          if ((dest_iter - prev_dir) != 3 ||
              prev_dir[0] != '.' ||
              prev_dir[1] != '.') {
            dest_iter = prev_dir;

            path_iter += 2;
            while (IS_PATH_SEP(*path_iter)) {
              ++path_iter;
            }
            continue;
          }
        }
      }
    }

    // Add the next directory to dest_iter.
    while (!IS_PATH_SEP(*path_iter) && *path_iter != '\0') {
      if (dest_iter == dest_end)
        return false;
      *dest_iter++ = *path_iter++;
    }
    if (dest_iter == dest_end && *path_iter != '\0')
      return false;
    *dest_iter++ = PATH_SEP;

    // collapse remaining path separators.
    while (IS_PATH_SEP(*path_iter)) {
      ++path_iter;
    }
  }

  // If the path ends with a slash and it is not the very first character,
  // remove the slash.
  if ((dest_iter - dest) > 1 && IS_PATH_SEP(*(dest_iter - 1))) {
    --dest_iter;
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
  if (IsAbsPath(path)) {
    return NormPath(path, path_len, dest, dest_size, dest_len);
  }

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

// Directory Path
bool FilePath::DirPath(const char* path, size_t path_len,
                       char* dest, size_t dest_size, size_t* dest_len) {
  const char* path_iter = path + path_len;
  for(; path_iter != path; --path_iter) {
    if (IS_PATH_SEP(*path_iter)) {
      break;
    }
  }

  if (path_iter == path) {
    if (IS_PATH_SEP(*path)) {
      ++path_iter; // first character is a slash is a root special case.
    }
  }

  const size_t copy_len = path_iter - path;
  if (dest_size < (copy_len + 1))
    return false;

  if (dest_len)
    *dest_len = copy_len;

  memcpy(dest, path, copy_len);
  dest[copy_len] = '\0';
  return true;
}

// Directory Creation functions
bool FilePath::EnsureDirectoryExists(const char* path, size_t path_len) {
  if (Exists(path))
    return true;
  else if (path_len == 0)
    return false;

  char parent_dir[256];
  size_t parent_dir_len = 0;
  if (!DirPath(path, path_len,
               parent_dir, sizeof(parent_dir), &parent_dir_len)) {
    return false;
  }

  if (!EnsureDirectoryExists(parent_dir, parent_dir_len)) {
    return false;
  }

  if (!CreateDir(path)) {
    return false;
  }

  return true;
}

}} // namespace ycommon { namespace platform {
