#ifndef YCOMMON_YPLATFORM_FILEPATH_H
#define YCOMMON_YPLATFORM_FILEPATH_H

namespace YCommon { namespace YPlatform {

namespace FilePath {
  bool IsFile(const char* filepath);
  bool IsDir(const char* dirpath);
  bool Exists(const char* path);
  bool GetCurrentWorkingDirectory(char* dir, size_t dir_size,
                                  size_t* dest_len = NULL);

  // Join Paths
  bool JoinPaths(const char* dir, size_t dir_len,
                 const char* path, size_t path_len,
                 char* dest, size_t dest_size, size_t* dest_len = NULL);

  // Normalize Path
  bool NormPath(const char* path, size_t path_len,
                char* dest, size_t dest_size, size_t* dest_len = NULL);

  // Absolute Path
  bool AbsPath(const char* path, size_t path_len,
               char* dest, size_t dest_size, size_t* dest_len = NULL,
               const char* working_dir = NULL, size_t working_dir_len = 0);

  // Relative Path
  bool RelPath(const char* path, size_t path_len,
               const char* dir, size_t dir_len,
               char* dest, size_t dest_size, size_t* dest_len = NULL);

  // Directory Path
  bool DirPath(const char* path, size_t path_len,
               char* dest, size_t dest_size, size_t* dest_len = NULL);
} // namespace FilePath

}} // namespace YCommon { namespace YPlatform {

#endif // YCOMMON_YPLATFORM_FILEPATH_H
