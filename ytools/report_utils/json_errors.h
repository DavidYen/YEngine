#ifndef YTOOLS_REPORT_UTILS_JSON_ERRORS_H
#define YTOOLS_REPORT_UTILS_JSON_ERRORS_H

#include <string>

#include <rapidjson/document.h>
#include <rapidjson/error/error.h>

namespace ytools { namespace file_utils {
  class FileEnv;
}} // namespace ytools { namespace file_utils {

namespace ytools { namespace report_utils {

namespace JsonErrors {
  void OutputJsonError(const char* file_path,
                       const char* buffer,
                       size_t offset,
                       rapidjson::ParseErrorCode error_code);

  // Automatically parses a json file and reports any errors.
  bool ParseJsonFile(const char* file_path,
                     rapidjson::Document& doc,
                     file_utils::FileEnv* file_env = nullptr);

  // Converts a rapidjson value to a readable string.
  const char* StringifyValueType(const rapidjson::Value& value);

} // namespace JsonErrors {

}} // namespace ytools { namespace report_utils {

#endif // YTOOLS_REPORT_UTILS_JSON_ERRORS_H
