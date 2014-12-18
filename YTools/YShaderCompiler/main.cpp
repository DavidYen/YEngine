#include <YCommon/Headers/stdincludes.h>

#include <iostream>
#include <string>
#include <vector>

#include <flatbuffers/flatbuffers.h>
#include <gflags/gflags.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <schemas/shader_generated.h>

#include <YTools/YFileUtils/FileEnv.h>
#include <YTools/YFileUtils/FilePath.h>
#include <YTools/YFileUtils/FileStream.h>

#include "CompileShader.h"

static bool ValidateExistingFile(const char* flagname,
                                 const std::string& value) {
  if (!value.empty() && YTools::YFileUtils::FilePath::IsFile(value))
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

static bool ValidateOptLevel(const char* flagname,
                             int32_t value) {
  (void) flagname;
  if (value >= 1 && value <= 3)
    return true;

  std::cerr << "Optimization Level must be between -1 and 3: " << value;
  return false;
}

DEFINE_string(input_file, "", "Input Shader File to Compile.");
DEFINE_string(output_file, "", "Output Shader File Compiled.");
DEFINE_string(schema_file, "", "Flatbuffer Schema File.");
DEFINE_string(dep_file, "", "Dependency File.");
DEFINE_bool(debug, false, "Default shaders to have debug information.");
DEFINE_int32(opt_level, 3, "Optimization level between -1~3 where -1 is none.");

static const bool input_check =
    gflags::RegisterFlagValidator(&FLAGS_input_file, ValidateExistingFile);
static const bool schema_check =
    gflags::RegisterFlagValidator(&FLAGS_schema_file, ValidateExistingFile);

static const bool output_check =
    gflags::RegisterFlagValidator(&FLAGS_output_file, ValidateFlagExists);

static const bool opt_level_check =
    gflags::RegisterFlagValidator(&FLAGS_opt_level, ValidateOptLevel);

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  YTools::YFileUtils::FileEnv file_env;

  FILE* file = fopen(FLAGS_input_file.c_str(), "r");
  file_env.AddReadFile(FLAGS_input_file);

  static char readbuffer[4096];
  rapidjson::FileReadStream stream(file, readbuffer, sizeof(readbuffer));
  rapidjson::Document doc;
  doc.ParseStream<0>(stream);
  if (!doc.IsObject()) {
    std::cerr << "Could not parse json file: "
              << "\"" << FLAGS_input_file << "\"."
              << std::endl;
    return 1;
  }

  rapidjson::Value& name_value = doc["name"];
  if (!name_value.IsString()) {
    std::cerr << "Shader Object required \"name\" string field." << std::endl;
    return 1;
  }
  const char* name_string = name_value.GetString();

  rapidjson::Value& file_value = doc["file"];
  if (!name_value.IsString()) {
    std::cerr << "Shader Object required \"file\" string field." << std::endl;
    return 1;
  }
  const char* file_string = file_value.GetString();

  flatbuffers::FlatBufferBuilder fbb;

  // Compile the shader file.
  const std::string file_dir =
      YTools::YFileUtils::FilePath::DirPath(FLAGS_input_file);
  const std::string abs_shader_path =
      YTools::YFileUtils::FilePath::AbsPath(file_dir, file_string);

  // Create Variants Here
  rapidjson::Value& variants = doc["variants"];
  if (!variants.IsArray()) {
    std::cerr << "Shader File expected \"variants\" array." << std::endl;
    return 1;
  }

  std::vector< flatbuffers::Offset<YEngine::Variant> > variant_offsets;
  for (rapidjson::Value::ValueIterator variant_iter = variants.Begin();
       variant_iter != variants.End();
       ++variant_iter) {
    if (!variant_iter->IsNumber()) {
      std::cerr << "Variant Item expected to be an object." << std::endl;
      return 1;
    }

    rapidjson::Value& variant_name = (*variant_iter)["name"];
    rapidjson::Value& variant_vs = (*variant_iter)["vertex_shader"];
    rapidjson::Value& variant_ps = (*variant_iter)["pixel_shader"];
    rapidjson::Value& variant_target = (*variant_iter)["target"];
    rapidjson::Value& variant_def = (*variant_iter)["defines"];
    rapidjson::Value& variant_debug = (*variant_iter)["debug"];
    rapidjson::Value& variant_optlevel = (*variant_iter)["opt_level"];

    if (!variant_name.IsString()) {
      std::cerr << "No Valid Shader Variant Name Found." << std::endl;
      return 1;
    } else if (!variant_vs.IsString()) {
      std::cerr << "No Valid Shader Variant Vertex Shader Found." << std::endl;
      return 1;
    } else if (!variant_ps.IsString()) {
      std::cerr << "No Valid Shader Variant Pixel Shader Found." << std::endl;
      return 1;
    } else if (!variant_target.IsString()) {
      std::cerr << "No Valid Shader Variant Target Found." << std::endl;
      return 1;
    } else if (!variant_def.IsNull() && !variant_def.IsArray()) {
      std::cerr << "Shader Variant \"defines\" must an array." << std::endl;
      return 1;
    } else if (!variant_debug.IsNull() && !variant_debug.IsBool()) {
      std::cerr << "Shader Variant \"debug\" must be a bool." << std::endl;
      return 1;
    } else if (!variant_optlevel.IsNull() && !variant_optlevel.IsNumber()) {
      std::cerr << "Shader Variant \"opt_level\" must be an int." << std::endl;
      return 1;
    }

    std::vector<YTools::YShaderCompiler::ShaderDefine> defines;
    if (variant_def.IsArray()) {
      for (rapidjson::Value::ValueIterator iter = variant_def.Begin();
           iter != variant_def.End();
           ++iter) {
        rapidjson::Value& def_name = (*iter)["name"];
        rapidjson::Value& def_value = (*iter)["value"];
        if (!def_name.IsString()) {
          std::cerr << "Shader Define must have \"name\" string field."
                    << std::endl;
          return 1;
        } else if (!def_value.IsNull() && !def_value.IsString()) {
          std::cerr << "Shader Define \"value\" field expects a string."
                    << std::endl;
          return 1;
        }

        const char* name = def_name.GetString();
        const char* value = def_value.IsString() ? def_value.GetString() : "";
        defines.push_back({ name, value });
      }
    }

    bool debug = variant_debug.IsBool() ? variant_debug.GetBool() : FLAGS_debug;

    int opt_level = variant_optlevel.IsNumber() ?
                    variant_optlevel.GetInt() : FLAGS_opt_level;
    if (opt_level < -1 || opt_level > 3) {
      std::cerr << "Invalid Optimization Level for Variant: "
                << opt_level
                << std::endl;
      return 1;
    }
    YTools::YShaderCompiler::ShaderOpt shader_opt =
        static_cast<YTools::YShaderCompiler::ShaderOpt>(opt_level + 1);

    const char* target_string = variant_target.GetString();
    int major_version = atoi(target_string);
    int minor_version = 0;
    const char* dot_loc = strchr(target_string, '.');
    if (dot_loc != NULL) {
      minor_version = atoi(dot_loc + 1);
    }

    // Compile Vertex Shader
    std::vector<uint8_t> vs_output;
    if (!YTools::YShaderCompiler::CompileShader(
        abs_shader_path, variant_vs.GetString(),
        YTools::YShaderCompiler::kShaderType_VertexShader,
        major_version, minor_version, shader_opt, debug,
        defines.data(), defines.size(),
        vs_output, &file_env)) {
      std::cerr << "Vertex Shader Compilation failed." << std::endl;
      return 1;
    }

    // Compile Pixel Shader
    std::vector<uint8_t> ps_output;
    if (!YTools::YShaderCompiler::CompileShader(
        abs_shader_path, variant_ps.GetString(),
        YTools::YShaderCompiler::kShaderType_PixelShader,
        major_version, minor_version, shader_opt, debug,
        defines.data(), defines.size(),
        ps_output, &file_env)) {
      std::cerr << "Vertex Shader Compilation failed." << std::endl;
      return 1;
    }


    variant_offsets.push_back(YEngine::CreateVariant(
        fbb,
        fbb.CreateString(variant_name.GetString()),
        fbb.CreateVector(vs_output),
        fbb.CreateVector(ps_output)));
  }

  // Create Shader
  auto shader_loc = YEngine::CreateShader(fbb,
                                          fbb.CreateString(name_string),
                                          fbb.CreateVector(variant_offsets));

  // Finish building the shader
  YEngine::FinishShaderBuffer(fbb, shader_loc);

  YTools::YFileUtils::FileStream output_file(
      FLAGS_output_file,
      YTools::YFileUtils::FileStream::kFileMode_Write,
      YTools::YFileUtils::FileStream::kFileType_Binary,
      &file_env);

  if (!output_file.IsOpen()) {
    std::cerr << "Could not open output file: "
              << FLAGS_output_file
              << std::endl;
    return 1;
  }

  if (!output_file.Write(fbb.GetBufferPointer(), fbb.GetSize())) {
    std::cerr << "Could not write output file: "
              << FLAGS_output_file.c_str()
              << std::endl;
    return 1;
  }

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
