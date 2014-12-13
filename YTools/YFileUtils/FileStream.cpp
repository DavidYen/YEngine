#include <YCommon/Headers/stdincludes.h>
#include "FileStream.h"

#include "FileEnv.h"

namespace YTools { namespace YFileUtils {

FileStream::FileStream(FileEnv* file_env)
    : mFileEnv(file_env),
      mOpen(false),
      mMode(kFileMode_None),
      mType(kFileType_None) {
}

FileStream::FileStream(const std::string& file, FileMode mode, FileType type,
                       FileEnv* file_env)
    : mFileEnv(file_env),
      mOpen(false),
      mMode(kFileMode_None),
      mType(kFileType_None) {
  Open(file, mode, type);
}

FileStream::~FileStream() {}

void FileStream::Open(const std::string& file, FileMode mode, FileType type) {
  Close();

  mMode = mode;
  mType = type;

  if (mode == kFileMode_Read) {
    if (mFileEnv)
      mFileEnv->AddReadFile(file);
    std::ios_base::openmode mode = std::ofstream::in;
    switch (type) {
      case kFileType_Binary: mode |= std::ofstream::binary; break;
      default: break;
    }

    mInFile.open(file, mode);
    mOpen = mInFile.is_open();
  } else if (mode == kFileMode_Write) {
    if (mFileEnv)
      mFileEnv->AddWrittenFile(file);
    std::ios_base::openmode mode = std::ofstream::out;
    switch (type) {
      case kFileType_Binary: mode |= std::ofstream::binary; break;
      default: break;
    }

    mOutFile.open(file, mode);
    mOpen = mOutFile.is_open();
  }
}

void FileStream::Close() {
  if (mOpen) {
    switch (mMode) {
    case kFileMode_Read: mInFile.close(); break;
    case kFileMode_Write: mOutFile.close(); break;
    default: break;
    }

    mOpen = false;
  }
}

bool FileStream::Write(const void* data, size_t size) {
  if (mOpen && mMode == kFileMode_Write) {
    mOutFile.write(static_cast<const char*>(data), size);
    return true;
  }
  return false;
}

bool FileStream::Read(void* data, size_t size) {
  if (mOpen && mMode == kFileMode_Read) {
    mInFile.read(static_cast<char*>(data), size);
  }
  return false;
}

}} // namespace YTools { namespace YFileUtils {
