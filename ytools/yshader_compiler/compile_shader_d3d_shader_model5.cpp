#include "ytools/yshader_compiler/compile_shader.h"

#include <iostream>

#include "ytools/yshader_compiler/d3d_shader_compiler.h"

namespace ytools { namespace YShaderCompiler {

// Always support shader model 5.1
const char* kShaderTarget[NUM_SHADER_TYPES] = {
  // Vertex Shader Type.
  "vs_5_1",

  // Pixel Shader Type.
  "ps_5_1"
};

bool CompileShader(const std::string& source_file,
                   const std::string& entry_point,
                   ShaderType shader_type, int target_major, int target_minor,
                   ShaderOpt opt_level, bool debug,
                   const ShaderDefine* defines, size_t num_defines,
                   std::vector<uint8_t>& output,
                   file_utils::FileEnv* file_env) {
  if (shader_type < 0 || shader_type >= NUM_SHADER_TYPES) {
    std::cerr << "Invalid Shader Type: " << shader_type << std::endl;
    return false;
  }

  if (target_major != 5 || target_minor != 1) {
    std::cerr << "Unknown Shader Model: "
              << target_major << "." << target_minor << std::endl;
    return false;
  }

  const char* target_string = kShaderTarget[shader_type];
  return DXShaderCompiler::CompileShader(source_file, entry_point,
                                         target_string, opt_level, debug,
                                         defines, num_defines,
                                         output, file_env);
}

}} // namespace ytools { namespace YShaderCompiler {
