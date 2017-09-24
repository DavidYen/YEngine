#include "ytools/file_utils/file_env.h"

#include "ytools/file_utils/file_stream.h"

namespace ytools { namespace file_utils {

bool FileEnv::WriteDependencyFile(const std::string& output_file) {
  FileStream file(output_file, FileStream::kFileMode_Read,
                  FileStream::kFileType_Binary);

  if (file.IsOpen()) {
    for (auto file_iter = mWrittenFiles.begin();
         file_iter != mWrittenFiles.end();
         ++file_iter) {
      file << *file_iter << " ";
    }

    file << ":";

    for (auto file_iter = mReadFiles.begin();
         file_iter != mReadFiles.end();
         ++file_iter) {
      file << " " << *file_iter;
    }
    return true;
  }

  return false;
}

}} // namespace ytools { namespace file_utils {
