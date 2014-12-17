#include <YCommon/Headers/stdincludes.h>
#include "FilePath.h"

#include <YCommon/YPlatform/FilePath.h>

namespace YTools { namespace YFileUtils {

bool FilePath::IsFile(const std::string& filepath) {
  return YCommon::YPlatform::FilePath::IsFile(filepath.c_str());
}

bool FilePath::IsDir(const std::string& dirpath) {
  return YCommon::YPlatform::FilePath::IsDir(dirpath.c_str());
}

bool FilePath::Exists(const std::string& path) {
  return YCommon::YPlatform::FilePath::Exists(path.c_str());
}

std::string FilePath::GetCurrentWorkingDirectory() {
  char dir[256];
  if (YCommon::YPlatform::FilePath::GetCurrentWorkingDirectory(dir,
                                                               sizeof(dir))) {
    return std::string(dir);
  }
  return std::string();
}

// Join Paths
std::string FilePath::JoinPaths(const std::string& dir,
                                const std::string path) {
  char joined[256];
  if (YCommon::YPlatform::FilePath::JoinPaths(dir.c_str(), dir.length(),
                                              path.c_str(), path.length(),
                                              joined, sizeof(joined))) {
    return std::string(joined);
  }
  return std::string();
}

// Normalize Path
std::string FilePath::NormPath(const std::string& path) {
  char normpath[256];
  if (YCommon::YPlatform::FilePath::NormPath(path.c_str(), path.length(),
                                             normpath, sizeof(normpath))) {
    return std::string(normpath);
  }
  return std::string();
}

// Absolute Path
std::string FilePath::AbsPath(const std::string& path) {
  char abspath[256];
  if (YCommon::YPlatform::FilePath::AbsPath(path.c_str(), path.length(),
                                            abspath, sizeof(abspath))) {
    return std::string(abspath);
  }
  return std::string();
}

std::string FilePath::AbsPath(const std::string& path,
                              const std::string& working_dir) {
  char abspath[256];
  if (YCommon::YPlatform::FilePath::AbsPath(path.c_str(), path.length(),
                                            abspath, sizeof(abspath), NULL,
                                            working_dir.c_str(),
                                            working_dir.length())) {
    return std::string(abspath);
  }
  return std::string();
}

// Relative Path
std::string FilePath::RelPath(const std::string& path,
                              const std::string& dir) {
  char relpath[256];
  if (YCommon::YPlatform::FilePath::RelPath(path.c_str(), path.length(),
                                            dir.c_str(), dir.length(),
                                            relpath, sizeof(relpath))) {
    return std::string(relpath);
  }
  return std::string();
}

// Directory Path
std::string DirPath(const std::string& path) {
  char dirpath[256];
  if (YCommon::YPlatform::FilePath::DirPath(path.c_str(), path.length(),
                                            dirpath, sizeof(dirpath))) {
    return std::string(dirpath);
  }
  return std::string();
}

}} // namespace YTools { namespace YFileUtils {
