#include <YCommon/Headers/stdincludes.h>

#include <iostream>
#include <string>
#include <vector>

#include <flatbuffers/flatbuffers.h>
#include <gflags/gflags.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <schemas/shader_generated.h>

#include <YCommon/YPlatform/FilePath.h>
#include <YTools/YFileUtils/FileEnv.h>
#include <YTools/YFileUtils/FileStream.h>

static bool ValidateExistingFile(const char* flagname,
                                 const std::string& value) {
  if (YCommon::YPlatform::FilePath::IsFile(value.c_str()))
    return true;

  std::cerr << "Invalid value for --" << flagname << "." << std::endl;
  std::cerr << "  Expected valid file: " << value << std::endl;
  return false;
}

static bool ValidateFlagExists(const char* flagname,
                               const std::string& value) {
  (void) flagname;
  return !value.empty();
}

DEFINE_string(input_file, "", "Input Shader File to Compile.");
DEFINE_string(output_file, "", "Output Shader File Compiled.");
DEFINE_string(schema_file, "", "Flatbuffer Schema File.");
DEFINE_string(dep_file, "", "Dependency File.");

static const bool input_check =
    gflags::RegisterFlagValidator(&FLAGS_input_file, ValidateExistingFile);
static const bool schema_check =
    gflags::RegisterFlagValidator(&FLAGS_schema_file, ValidateExistingFile);

static const bool output_check =
    gflags::RegisterFlagValidator(&FLAGS_output_file, ValidateFlagExists);

using namespace rapidjson;

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  YTools::YFileUtils::FileEnv file_env;

  FILE* file = fopen(FLAGS_input_file.c_str(), "r");
  file_env.AddReadFile(FLAGS_input_file);

  static char readbuffer[4096];
  rapidjson::FileReadStream stream(file, readbuffer, sizeof(readbuffer));
  rapidjson::Document doc;
  doc.ParseStream<0>(stream);
  YASSERT(doc.IsObject(),
          "Could not parse json file: \"%s\".",
          FLAGS_input_file.c_str());

  flatbuffers::FlatBufferBuilder fbb;

  YASSERT(doc.HasMember("name"),
          "Shader file does not have name field: %s",
          FLAGS_input_file.c_str());
  auto shader_name = fbb.CreateString(doc["name"].GetString());

  // Compile the shader file.
  YASSERT(doc.HasMember("file"),
          "Shader file does not have attached HLSL file: %s",
          FLAGS_input_file.c_str());


  // Create Variants Here

  // Create Shader
  auto shader_loc = YEngine::CreateShader(fbb, shader_name);

  // Finish building the shader
  YEngine::FinishShaderBuffer(fbb, shader_loc);

  YTools::YFileUtils::FileStream output_file(
      FLAGS_output_file,
      YTools::YFileUtils::FileStream::kFileMode_Write,
      YTools::YFileUtils::FileStream::kFileType_Binary,
      &file_env);

  YASSERT(output_file.IsOpen(),
          "Could not open output file: %s",
          FLAGS_output_file.c_str());

  bool written = output_file.Write(fbb.GetBufferPointer(), fbb.GetSize());
  (void) written;
  YASSERT(written,
          "Could not write output file: %s",
          FLAGS_output_file.c_str());

  if (!FLAGS_dep_file.empty()) {
    if (!file_env.WriteDependencyFile(FLAGS_dep_file)) {
      std::cerr << "[Error] Could not write to dependency file: "
                << '"' << FLAGS_dep_file << '"'
                << std::endl;
      return 1;
    }
  }

  return 0;
}
