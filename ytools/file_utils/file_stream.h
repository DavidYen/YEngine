#ifndef YTOOLS_FILE_UTILS_FILE_STREAM_H
#define YTOOLS_FILE_UTILS_FILE_STREAM_H

#include <fstream>
#include <string>
#include <vector>

namespace ytools { namespace file_utils {

class FileEnv;

class FileStream {
 public:
  enum FileMode {
    kFileMode_None,
    kFileMode_Read,
    kFileMode_Write
  };

  enum FileType {
    kFileType_None,
    kFileType_Binary,
    kFileType_Text
  };

  FileStream(FileEnv* file_env = nullptr);
  FileStream(const std::string& file, FileMode mode, FileType type,
             FileEnv* file_env = nullptr);
  ~FileStream();

  void Open(const std::string& file, FileMode mode, FileType type);
  void Close();

  bool IsOpen() { return mOpen; }

  bool Write(const void* data, size_t size);
  bool Read(void* data, size_t size);

  template <typename VECTOR_TYPE>
  bool Write(const VECTOR_TYPE& data) {
    if (mOpen && mMode == kFileMode_Write) {
      mOutFile.write(reinterpret_cast<const char*>(data.data()),
                     data.size() * sizeof(data[0]) );
      return true;
    }
    return false;
  }

  template <typename VECTOR_TYPE>
  bool Read(VECTOR_TYPE& data) {
    if (mOpen && mMode == kFileMode_Read) {
      const std::streamoff current_pos = mInFile.tellg();
      mInFile.seekg(0, std::ios_base::end);
      const std::streamoff length = mInFile.tellg() / sizeof(data[0]);

      mInFile.seekg(current_pos, std::ios_base::beg);

      data.resize(static_cast<size_t>(length));
      mInFile.read(reinterpret_cast<char*>(data.data()), length);
      return true;
    }
    return false;
  }

  template <typename T>
  std::ostream& operator<<(const T& obj)  {
    if (mOpen && mMode == kFileMode_Write) {
      mOutFile << obj;
    }
    return mOutFile;
  }

  template <typename T>
  std::istream& operator>>(T& obj) {
    if (mOpen && mMode == kFileMode_Read) {
      obj >> mInFile;
    }
    return mInFile;
  }

  std::ifstream& GetInStream() { return mInFile; }
  std::ofstream& GetOutStream() { return mOutFile; }

 private:
  FileEnv* mFileEnv;
  bool mOpen;
  FileMode mMode;
  FileType mType;

  std::ifstream mInFile;
  std::ofstream mOutFile;
};

}} // namespace ytools { namespace file_utils {

#endif // YTOOLS_FILE_UTILS_FILE_STREAM_H
