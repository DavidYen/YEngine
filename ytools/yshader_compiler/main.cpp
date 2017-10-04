
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <flatbuffers/flatbuffers.h>
#include <gflags/gflags.h>
#include <rapidjson/document.h>
#include <schemas/shader_generated.h>

#include "ytools/file_utils/file_env.h"
#include "ytools/file_utils/file_path.h"
#include "ytools/file_utils/file_stream.h"
#include "ytools/report_utils/gflags_validators.h"
#include "ytools/report_utils/json_errors.h"
#include "ytools/yshader_compiler/compile_shader.h"

static bool ValidateOptLevel(const char* /* flagname */,
                      int32_t value) {
  if (value >= 1 && value <= 3)
    return true;

  std::cerr << "Optimization Level must be between -1 and 3: " << value;
  return false;
}

DEFINE_bool(verbose, false, "Verbose Output");
DEFINE_string(input_file, "", "Input Shader File to Compile.");
DEFINE_string(output_file, "", "Output Shader File Compiled.");
DEFINE_string(dep_file, "", "Dependency File.");
DEFINE_bool(debug, false, "Default shaders to have debug information.");
DEFINE_int32(opt_level, 3, "Optimization level between -1~3 where -1 is none.");

static const bool input_check = gflags::RegisterFlagValidator(
  &FLAGS_input_file,
  ytools::report_utils::GFlagsValidators::ValidateExistingFile);

static const bool output_check = gflags::RegisterFlagValidator(
  &FLAGS_output_file,
  ytools::report_utils::GFlagsValidators::ValidateFlagExists);

static const bool opt_level_check = gflags::RegisterFlagValidator(
  &FLAGS_opt_level, ValidateOptLevel);

namespace {
  class ShaderParamTypeMap
  {
   public:
    ShaderParamTypeMap() {
      for (int i = static_cast<int>(yengine_data::ParamType::MIN);
           i <= static_cast<int>(yengine_data::ParamType::MAX);
           ++i) {
        yengine_data::ParamType param_type =
            static_cast<yengine_data::ParamType>(i);
        mParamTypeMap[yengine_data::EnumNameParamType(param_type)] = param_type;
      }
    }
    ~ShaderParamTypeMap() = default;

    yengine_data::ParamType operator[](const std::string& param_type) const {
      const auto& iter = mParamTypeMap.find(param_type);
      return (iter == mParamTypeMap.end())
             ? yengine_data::ParamType::kInvalid
             : iter->second;
    }

   private:
    std::unordered_map<std::string, yengine_data::ParamType> mParamTypeMap;
  };

  ShaderParamTypeMap gParamTypeMap;

  uint8_t kMaxOffsetValue[] = {
    0, // kInvalid,
    3, // kFloat,
    2, // kFloat2,
    1, // kFloat3,
    0, // kTexture,
    0, // kSampler,
  };
  static_assert(sizeof(kMaxOffsetValue) ==
                static_cast<size_t>(yengine_data::ParamType::MAX) + 1,
                "kMaxOffsetValue must correspond to each param type.");
}

bool ParseJsonUint8(const rapidjson::Value& value,
                    uint8_t& number,
                    uint8_t max_value) {
  if (!value.IsInt()) {
    std::cerr << "Expected integer type." << std::endl;
    return false;
  }
  if (value.IsInt()) {
    if (value.GetInt() < 0 || value.GetInt() > max_value) {
      std::cerr << "Invalid integer value: "
                << "(0 <= " << value.GetInt() << " <= "
                << static_cast<int>(max_value) << ")"
                << std::endl;
      return false;
    }
    number = static_cast<uint8_t>(value.GetInt());
  }

  return true;
}

bool GetShaderParams(
    const rapidjson::Value& parameters_list,
    flatbuffers::FlatBufferBuilder& fbb,
    std::vector< flatbuffers::Offset<yengine_data::ParamData> >& params) {
  if (!parameters_list.IsArray()) {
    std::cerr << "Shader has invalid parameters." << std::endl;
    return false;
  }

  std::unordered_set<std::string> param_names;
  for (const rapidjson::Value& param : parameters_list.GetArray()) {
    if (!param.IsObject()) {
      std::cerr << "Shader Parameter is not an object." << std::endl;
      return false;
    }

    // Parse the name.
    const rapidjson::Value::ConstMemberIterator name_iter =
        param.FindMember("name");
    if (name_iter == param.MemberEnd()) {
      std::cerr << "Shader Parameter has no name associated to it."
                << std::endl;
      return false;
    }

    const rapidjson::Value& param_name = name_iter->value;
    if (!param_name.IsString()) {
      std::cerr << "Shader Parameter has an invalid name associated to it."
                << std::endl;
      return false;
    }

    const auto& insert_pair = param_names.insert(param_name.GetString());
    if (!insert_pair.second) {
      std::cerr << "Shader Parameter \"" << param_name.GetString() << "\""
                << " is defined multiple times."
                << std::endl;
      return false;
    }

    // Parse the type.
    const rapidjson::Value::ConstMemberIterator type_iter =
        param.FindMember("type");
    if (name_iter == param.MemberEnd()) {
      std::cerr << "Shader Parameter \"" << param_name.GetString() << "\""
                << " is missing the \"type\" field."
                << std::endl;
      return false;
    }

    const rapidjson::Value& type_name = type_iter->value;
    if (!type_name.IsString()) {
      std::cerr << "Shader Parameter \"" << param_name.GetString() << "\""
                << " has invalid \"type\" value."
                << std::endl;
      return false;
    }

    yengine_data::ParamType param_type = gParamTypeMap[type_name.GetString()];
    if (param_type == yengine_data::ParamType::kInvalid) {
      std::cerr << "Shader Parameter \"" << param_name.GetString() << "\""
                << " han unknown \"type\" value: " << type_name.GetString()
                << std::endl;
      return false;
    }

    // Parse the Register information
    uint8_t register_index = 0;
    uint8_t register_offset = 0;
    const rapidjson::Value::ConstMemberIterator register_iter =
        param.FindMember("register");
    if (register_iter != param.MemberEnd()) {
      const rapidjson::Value& register_data = register_iter->value;
      if (!register_data.IsObject()) {
        std::cerr << "Shader Parameter \"" << param_name.GetString() << "\""
                  << " has invalid \"register\" value."
                  << std::endl;
        return false;
      }

      const rapidjson::Value::ConstMemberIterator index_iter =
          register_data.FindMember("index");
      if (index_iter != register_data.MemberEnd() &&
          !ParseJsonUint8(index_iter->value, register_index, UCHAR_MAX)) {
        std::cerr << "Shader Parameter \"" << param_name.GetString() << "\""
                  << " has invalid register index value."
                  << std::endl;
        return false;
      }

      const rapidjson::Value::ConstMemberIterator offset_iter =
          register_data.FindMember("offset");
      if (offset_iter != register_data.MemberEnd() &&
          !ParseJsonUint8(offset_iter->value,
                          register_offset,
                          kMaxOffsetValue[static_cast<int>(param_type)])) {
        std::cerr << "Shader Parameter \"" << param_name.GetString() << "\""
                  << " has invalid register offset value."
                  << std::endl;
        return false;
      }
    }

    const yengine_data::RegisterOffset register_offset_data(register_index,
                                                            register_offset);
    params.push_back(CreateParamData(fbb,
                                     fbb.CreateString(type_name.GetString()),
                                     param_type,
                                     &register_offset_data));
  }
  return true;
}

bool CompileShaderBinary(ytools::file_utils::FileEnv* file_env,
                         const rapidjson::Value& variant,
                         const std::string& shader_path,
                         std::vector<uint8_t>& vs_output,
                         std::vector<uint8_t>& ps_output) {
  // Get the Vertex Shader name.
  const rapidjson::Value::ConstMemberIterator vs_iter =
      variant.FindMember("vertex_shader");
  if (vs_iter == variant.MemberEnd() && !vs_iter->value.IsString()) {
    std::cerr << "No Valid Shader Variant Vertex Shader Found." << std::endl;
    return false;
  }
  const char* variant_vs = vs_iter->value.GetString();

  // Get the Pixel Shader name.
  const rapidjson::Value::ConstMemberIterator ps_iter =
      variant.FindMember("pixel_shader");
  if (ps_iter == variant.MemberEnd() && !ps_iter->value.IsString()) {
    std::cerr << "No Valid Shader Variant Pixel Shader Found." << std::endl;
    return false;
  }
  const char* variant_ps = ps_iter->value.GetString();

  // Get the variant target.
  const rapidjson::Value::ConstMemberIterator target_iter =
      variant.FindMember("target");
  if (target_iter == variant.MemberEnd() && !target_iter->value.IsString()) {
    std::cerr << "No Valid Shader Variant Target Found." << std::endl;
    return false;
  }
  const char* variant_target = target_iter->value.GetString();

  // Get the defines.
  std::vector<ytools::YShaderCompiler::ShaderDefine> defines;
  const rapidjson::Value::ConstMemberIterator defines_iter =
      variant.FindMember("defines");
  if (defines_iter != variant.MemberEnd()) {
    if (!defines_iter->value.IsArray()) {
      std::cerr << "Shader Variant \"defines\" must an array." << std::endl;
      return false;
    }
    for (const rapidjson::Value& define_iter : defines_iter->value.GetArray()) {
      if (!define_iter.IsObject()) {
        std::cerr << "Shader Define expected to be an object of name and value."
                  << std::endl;
        return false;
      }

      // Define Name.
      const rapidjson::Value::ConstMemberIterator name_iter =
          define_iter.FindMember("name");
      if (name_iter == define_iter.MemberEnd() ||
          !name_iter->value.IsString()) {
        std::cerr << "Shader Define must have \"name\" string field."
                  << std::endl;
        return false;
      }
      const char* define_name = name_iter->value.GetString();

      // Define Value.
      const char* define_value = "";
      const rapidjson::Value::ConstMemberIterator value_iter =
          define_iter.FindMember("value");
      if (value_iter != define_iter.MemberEnd()) {
        if (!value_iter->value.IsString()) {
          std::cerr << "Shader Define \"value\" field expects a string."
                    << std::endl;
          return false;
        }
        define_value = value_iter->value.GetString();
      }

      defines.push_back({ define_name, define_value });
    }
  }

  // Get the debug value.
  bool debug = FLAGS_debug;
  const rapidjson::Value::ConstMemberIterator debug_iter =
      variant.FindMember("debug");
  if (debug_iter != variant.MemberEnd()) {
    if (!debug_iter->value.IsBool()) {
      std::cerr << "Shader Variant \"debug\" must be a bool." << std::endl;
      return false;
    }
    debug = debug_iter->value.GetBool();
  }

  // Get the opt_level value.
  int opt_level = FLAGS_opt_level;
  const rapidjson::Value::ConstMemberIterator opt_level_iter =
      variant.FindMember("opt_level");
  if (opt_level_iter != variant.MemberEnd()) {
    if (!opt_level_iter->value.IsNumber()) {
      std::cerr << "Shader Variant \"opt_level\" must be an int." << std::endl;
      return false;
    }
    opt_level = opt_level_iter->value.GetInt();
  }

  if (opt_level < -1 || opt_level > 3) {
    std::cerr << "Invalid Optimization Level for Shader: "
              << opt_level
              << std::endl;
    return false;
  }

  ytools::YShaderCompiler::ShaderOpt shader_opt =
      static_cast<ytools::YShaderCompiler::ShaderOpt>(opt_level + 1);

  int major_version = atoi(variant_target);
  int minor_version = 0;
  const char* dot_loc = strchr(variant_target, '.');
  if (dot_loc != NULL) {
    minor_version = atoi(dot_loc + 1);
  }

  // Compile Vertex Shader
  if (!ytools::YShaderCompiler::CompileShader(
      shader_path, variant_vs,
      ytools::YShaderCompiler::kShaderType_VertexShader,
      major_version, minor_version, shader_opt, debug,
      defines.data(), defines.size(),
      vs_output, file_env)) {
    std::cerr << "Vertex Shader Compilation failed." << std::endl;
    return false;
  }

  // Compile Pixel Shader
  if (!ytools::YShaderCompiler::CompileShader(
      shader_path, variant_ps,
      ytools::YShaderCompiler::kShaderType_PixelShader,
      major_version, minor_version, shader_opt, debug,
      defines.data(), defines.size(),
      ps_output, file_env)) {
    std::cerr << "Vertex Shader Compilation failed." << std::endl;
    return false;
  }

  return true;
}

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  ytools::file_utils::FileEnv file_env;

  file_env.AddReadFile(FLAGS_input_file);
  rapidjson::Document doc;
  if (!ytools::report_utils::JsonErrors::ParseJsonFile(FLAGS_input_file.c_str(),
                                                       doc)) {
    return 1;
  }

  if (!doc.IsObject()) {
    std::cerr << "Expected top level dict object: "
              << "\"" << FLAGS_input_file << "\"."
              << std::endl;
    return 1;
  }

  const rapidjson::Value::ConstMemberIterator name_iter =
      doc.FindMember("name");
  if (name_iter == doc.MemberEnd() || !name_iter->value.IsString()) {
    std::cerr << "Shader Object requires \"name\" string field." << std::endl;
    return 1;
  }
  const char* name_string = name_iter->value.GetString();

  const rapidjson::Value::ConstMemberIterator file_iter =
      doc.FindMember("file");
  if (file_iter == doc.MemberEnd() || !file_iter->value.IsString()) {
    std::cerr << "Shader Object requires \"file\" string field." << std::endl;
    return 1;
  }
  const char* file_string = file_iter->value.GetString();

  flatbuffers::FlatBufferBuilder fbb;

  // Compile the shader file.
  const std::string file_dir =
      ytools::file_utils::FilePath::DirPath(FLAGS_input_file);
  const std::string abs_shader_path =
      ytools::file_utils::FilePath::AbsPath(file_string, file_dir);

  if (FLAGS_verbose) {
    std::cout << "Compiling Shader [" << name_string << "] "
              << file_string << "..." << std::endl;
  }

  // Parse Variants
  const rapidjson::Value::ConstMemberIterator variants_iter =
      doc.FindMember("variants");
  if (variants_iter == doc.MemberEnd() || !variants_iter->value.IsArray()) {
    std::cerr << "Shader File expected \"variants\" array." << std::endl;
    return 1;
  }

  std::vector< flatbuffers::Offset<yengine_data::Variant> > variant_offsets;
  for (const rapidjson::Value& variant : variants_iter->value.GetArray()) {
    if (!variant.IsObject()) {
      std::cerr << "Variant Item expected to be an object." << std::endl;
      return 1;
    }

    // Parse Variant Name.
    const rapidjson::Value::ConstMemberIterator variant_name_iter =
        variant.FindMember("name");
    if (variant_name_iter == variant.MemberEnd() ||
        !variant_name_iter->value.IsString()) {
      std::cerr << "No Valid Shader Variant Name Found." << std::endl;
      return 1;
    }
    const char* variant_name = variant_name_iter->value.GetString();

    if (FLAGS_verbose) {
      std::cout << "Compiling Variant: "
                << variant_name
                << std::endl;
    }

    // Parse Vertex Declaration
    const rapidjson::Value::ConstMemberIterator variant_vertex_decl_iter =
        variant.FindMember("vertex_declaration");
    if (variant_vertex_decl_iter == variant.MemberEnd() ||
        !variant_vertex_decl_iter->value.IsString()) {
      std::cerr << "Shader Variant \"" << variant_name
                << "\" has no valid \"vertex_declaration\" field."
                << std::endl;
      return 1;
    }
    const char* vertex_decl = variant_vertex_decl_iter->value.GetString();

    std::vector<uint8_t> vs_output;
    std::vector<uint8_t> ps_output;
    if (!CompileShaderBinary(&file_env, variant,
                             abs_shader_path, vs_output, ps_output)) {
      return 1;
    }

    const rapidjson::Value::ConstMemberIterator vertex_param_iter =
        variant.FindMember("vertex_parameters");
    std::vector< flatbuffers::Offset<yengine_data::ParamData> > vs_params;
    if (vertex_param_iter != variant.MemberEnd() &&
        !GetShaderParams(vertex_param_iter->value, fbb, vs_params)) {
      return 1;
    }

    const rapidjson::Value::ConstMemberIterator pixel_param_iter =
        variant.FindMember("pixel_parameters");
    std::vector< flatbuffers::Offset<yengine_data::ParamData> > ps_params;
    if (pixel_param_iter != variant.MemberEnd() &&
        !GetShaderParams(pixel_param_iter->value, fbb, ps_params)) {
      return 1;
    }

    variant_offsets.push_back(yengine_data::CreateVariant(
        fbb,
        fbb.CreateString(variant_name),
        fbb.CreateString(vertex_decl),
        fbb.CreateVector(vs_output),
        fbb.CreateVector(vs_params),
        fbb.CreateVector(ps_output),
        fbb.CreateVector(ps_params)));
  }

  // Create Shader
  auto shader_loc = yengine_data::CreateShader(
      fbb, fbb.CreateString(name_string), fbb.CreateVector(variant_offsets));

  // Finish building the shader
  yengine_data::FinishShaderBuffer(fbb, shader_loc);

  ytools::file_utils::FileStream output_file(
      FLAGS_output_file,
      ytools::file_utils::FileStream::kFileMode_Write,
      ytools::file_utils::FileStream::kFileType_Binary,
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
