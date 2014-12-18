#include <YCommon/Headers/stdincludes.h>
#include "CompileShader.h"

#include <iostream>

#include "DXShaderCompiler.h"

namespace YTools { namespace YShaderCompiler {

#define MAX_MAJOR_NUM 4

const char* kShaderTarget[MAX_MAJOR_NUM+1][NUM_SHADER_TYPES] = {
  // 0.0 is unsupported
  { },

  // 1.0 is not supported
  { },

  // 2.0
  { "vs_2_0", "ps_2_0" },

  // 3.0
  { "vs_3_0", "ps_3_0" },

  // 4.0
  { "vs_4_0_level_9_3", "ps_4_0_level_9_3" },
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

  if (target_major < 0 || target_major > ARRAY_SIZE(kShaderTarget) ||
      target_minor != 0 || kShaderTarget[target_major][shader_type] == NULL) {
    std::cerr << "DX9 does not support shader model: "
              << target_major << "." << target_minor << std::endl;
    return false;
  }

  const char* target_string = kShaderTarget[target_major][shader_type];
  return DXShaderCompiler::CompileShader(source_file, entry_point,
                                         target_string, opt_level, debug,
                                         defines, num_defines,
                                         output, file_env);
}

}} // namespace YTools { namespace YShaderCompiler {
