#ifndef YTOOLS_FILE_UTILS_FILE_ENV_H
#define YTOOLS_FILE_UTILS_FILE_ENV_H

#include <string>
#include <vector>

namespace ytools { namespace file_utils {

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

}} // namespace ytools { namespace file_utils {

#endif // YTOOLS_FILE_UTILS_FILE_ENV_H
