#ifndef YTOOLS_YFILEUTILS_FILEPATH_H
#define YTOOLS_YFILEUTILS_FILEPATH_H

#include <string>

namespace YTools { namespace YFileUtils {

namespace FilePath {
  bool IsFile(const std::string& filepath);
  bool IsDir(const std::string& dirpath);
  bool Exists(const std::string& path);
  std::string GetCurrentWorkingDirectory();

  // Join Paths
  std::string JoinPaths(const std::string& dir, const std::string path);

  // Normalize Path
  std::string NormPath(const std::string& path);

  // Absolute Path
  std::string AbsPath(const std::string& path);
  std::string AbsPath(const std::string& path, const std::string& working_dir);

  // Relative Path
  std::string RelPath(const std::string& path, const std::string& dir);

  // Directory Path
  std::string DirPath(const std::string& path);

} // namespace FilePath {

}} // namespace YTools { namespace YFileUtils {

#endif // YTOOLS_YFILEUTILS_FILEPATH_H
