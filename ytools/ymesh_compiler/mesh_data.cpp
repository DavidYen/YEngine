#include "ytools/ymesh_compiler/mesh_data.h"

#include <cstdio>
#include <iostream>

#include <rapidjson/document.h>
#include <schemas/mesh_generated.h>

#include "ytools/file_utils/file_env.h"
#include "ytools/file_utils/file_stream.h"
#include "ytools/report_utils/json_errors.h"
#include "ytools/ymesh_compiler/mesh_data_inline.h"

namespace ytools { namespace ymesh_compiler {

MeshData::MeshData(ytools::file_utils::FileEnv* file_env,
                   const std::string& file_path) {
  mValid = ProcessMeshData(file_env, file_path);
}

bool MeshData::WriteModelBinaryFile(ytools::file_utils::FileEnv* file_env,
                                    const std::string& file_path) {
  ytools::file_utils::FileStream output_file(
      file_path,
      ytools::file_utils::FileStream::kFileMode_Write,
      ytools::file_utils::FileStream::kFileType_Binary,
      file_env
  );

  if (!output_file.IsOpen()) {
    std::cerr << "Could not open output file: " << file_path << std::endl;
    return false;
  }

  if (!output_file.Write(mMeshData.data(), mMeshData.size())) {
    std::cerr << "Could not write output file: " << file_path << std::endl;
    return false;
  }

  return true;
}

bool MeshData::ProcessMeshData(ytools::file_utils::FileEnv* file_env,
                               const std::string& file_path) {
  rapidjson::Document doc;
  if (!report_utils::JsonErrors::ParseJsonFile(file_path.c_str(),
                                               doc,
                                               file_env)) {
    return false;
  }

  const auto& name_iter = doc.FindMember("name");
  if (name_iter != doc.MemberEnd() &&
      !name_iter->value.IsString()) {
    std::cerr << "Mesh JSON expected a string \"name\" field." << std::endl;
    return false;
  }
  const char* name = name_iter->value.GetString();

  const auto& input_type_iter = doc.FindMember("input_type");
  if (input_type_iter != doc.MemberEnd() &&
      !input_type_iter->value.IsString()) {
    std::cerr << "Mesh JSON expected a string \"input_type\" field."
              << std::endl;
    return false;
  }

  const char* inline_type = input_type_iter->value.GetString();
  if (strcmp("inline", inline_type) == 0) {
    return MeshDataInline::ProcessInlineData(name, doc, mMeshData);
  } else {
    std::cerr << "Unknown mesh input type: " << inline_type << std::endl;
    return false;
  }
}

}} // namespace ytools { namespace ymesh_compiler {
