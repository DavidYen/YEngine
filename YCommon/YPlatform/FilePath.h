#ifndef YCOMMON_YPLATFORM_FILEPATH_H
#define YCOMMON_YPLATFORM_FILEPATH_H

namespace YCommon { namespace YPlatform {

namespace FilePath {
  bool IsFile(const char* filepath);
  bool IsDir(const char* dirpath);
  bool Exists(const char* path);
} // namespace FilePath

}} // namespace YCommon { namespace YPlatform {

#endif // YCOMMON_YPLATFORM_FILEPATH_H
