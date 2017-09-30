#include "ytools/report_utils/json_errors.h"

#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/filereadstream.h>

#include "ytools/file_utils/file_env.h"

namespace ytools { namespace report_utils {

void JsonErrors::OutputJsonError(const char* file_path,
                                 const char* buffer,
                                 size_t offset,
                                 rapidjson::ParseErrorCode error_code) {
  size_t line_num = 1;
  size_t index = 0;
  for (size_t i = 0; i < offset; ++i) {
    if (buffer[i] == '\n') {
      ++line_num;
      index = 0;
    } else {
      ++index;
    }
  }

  std::cerr << "Json Parsing Error "
            << file_path
            << ":"
            << line_num
            << ":"
            << index
            << " - "
            << rapidjson::GetParseError_En(error_code)
            << std::endl;
}

bool JsonErrors::ParseJsonFile(const char* file_path,
                               rapidjson::Document& doc,
                               file_utils::FileEnv* file_env) {
  if (file_env) {
    file_env->AddReadFile(file_path);
  }

  static char readbuffer[4096];
  FILE* file = fopen(file_path, "rb");
  rapidjson::FileReadStream stream(file, readbuffer, sizeof(readbuffer));
  doc.ParseStream<0>(stream);
  fclose(file);
  if (doc.HasParseError()) {
    OutputJsonError(file_path,
                    readbuffer,
                    doc.GetErrorOffset(),
                    doc.GetParseError()
    );
    std::cerr << "Could not parse json file: "
              << "\"" << file_path << "\"."
              << std::endl;
    return false;
  }
  return true;
}

}} // namespace ytools { namespace report_utils {
