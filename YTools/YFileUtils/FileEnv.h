#ifndef YTOOLS_YFILEUTILS_FILEENV_H
#define YTOOLS_YFILEUTILS_FILEENV_H

#include <string>
#include <vector>

namespace YTools { namespace YFileUtils {

class FileEnv {
 public:
  FileEnv() {}
  ~FileEnv() {}

  void AddReadFile(const std::string& file) {
    mReadFiles.push_back(file);
  }

  void AddWrittenFile(const std::string& file) {
    mWrittenFiles.push_back(file);
  }

  const std::vector<std::string>& GetReadFiles() { return mReadFiles; }
  const std::vector<std::string>& GetWrittenFiles() { return mWrittenFiles; }

  bool WriteDependencyFile(const std::string& output_file);

 private:
  std::vector<std::string> mReadFiles;
  std::vector<std::string> mWrittenFiles;
};

}} // namespace YTools { namespace YFileUtils {

#endif // YTOOLS_YFILEUTILS_FILEENV_H
