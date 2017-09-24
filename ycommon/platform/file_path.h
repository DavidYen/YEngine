#ifndef YCOMMON_PLATFORM_FILE_PATH_H
#define YCOMMON_PLATFORM_FILE_PATH_H

namespace ycommon { namespace platform {

namespace FilePath {
  /********
  * Platform Dependent Functions.
  *********/
  bool IsAbsPath(const char* path);
  bool IsFile(const char* filepath);
  bool IsDir(const char* dirpath);
  bool Exists(const char* path);

  bool GetCurrentWorkingDirectory(char* dir, size_t dir_size,
                                  size_t* dest_len = nullptr);

  /********
  * Platform Independent Functions.
  *********/
  // Join Paths
  bool JoinPaths(const char* dir, size_t dir_len,
                 const char* path, size_t path_len,
                 char* dest, size_t dest_size, size_t* dest_len = nullptr);

  // Normalize Path
  bool NormPath(const char* path, size_t path_len,
                char* dest, size_t dest_size, size_t* dest_len = nullptr);

  // Absolute Path
  bool AbsPath(const char* path, size_t path_len,
               char* dest, size_t dest_size, size_t* dest_len = nullptr,
               const char* working_dir = nullptr, size_t working_dir_len = 0);

  // Relative Path
  bool RelPath(const char* path, size_t path_len,
               const char* dir, size_t dir_len,
               char* dest, size_t dest_size, size_t* dest_len = nullptr);

  // Directory Path
  bool DirPath(const char* path, size_t path_len,
               char* dest, size_t dest_size, size_t* dest_len = nullptr);
} // namespace FilePath

}} // namespace ycommon { namespace platform {

#endif // YCOMMON_PLATFORM_FILE_PATH_H
