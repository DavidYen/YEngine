#include "ytools/file_utils/file_env.h"

#include <iostream>

#include "ytools/file_utils/file_path.h"
#include "ytools/file_utils/file_stream.h"

namespace ytools { namespace file_utils {

bool FileEnv::WriteDependencyFile(const std::string& output_file) {
  // Make sure parent directory exists.
  const std::string output_dir = FilePath::DirPath(output_file);
  if (!FilePath::EnsureDirectoryExists(output_dir)) {
    std::cerr << "Could not create directory: " << output_dir << std::endl;
    return false;
  }

  FileStream file(output_file, FileStream::kFileMode_Write,
                  FileStream::kFileType_Binary);

  if (!file.IsOpen()) {
    std::cerr << "[Error] Could not write to dependency file: "
                << '"' << output_file << '"'
                << std::endl;
    return false;
  }

  for (auto file_iter = mWrittenFiles.begin();
       file_iter != mWrittenFiles.end();
       ++file_iter) {
    file << *file_iter << " ";
  }

  file << ": \\" << std::endl;

  for (auto file_iter = mReadFiles.begin();
       file_iter != mReadFiles.end();
       ++file_iter) {
    file << "  " << *file_iter << " \\" << std::endl;
  }
  return true;
}

}} // namespace ytools { namespace file_utils {
