#include <YCommon/Headers/stdincludes.h>
#include "CompileShader.h"

#include <iostream>

#include "DXShaderCompiler.h"

namespace YTools { namespace YShaderCompiler {

// Always support shader model 4.0
const char* kShaderTarget[NUM_SHADER_TYPES] = {
  // Vertex Shader Type.
  "vs_4_0_level_9_3",

  // Pixel Shader Type.
  "ps_4_0_level_9_3"
};

bool CompileShader(const std::string& source_file,
                   const std::string& entry_point,
                   ShaderType shader_type, int target_major, int target_minor,
                   ShaderOpt opt_level, bool debug,
                   const ShaderDefine* defines, size_t num_defines,
                   std::vector<uint8_t>& output,
                   YFileUtils::FileEnv* file_env) {
  if (shader_type < 0 || shader_type >= NUM_SHADER_TYPES) {
    std::cerr << "Invalid Shader Type: " << shader_type << std::endl;
    return false;
  }

  if (target_major < 0 || target_major > 4 || target_minor != 0) {
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

}} // namespace YTools { namespace YShaderCompiler {
