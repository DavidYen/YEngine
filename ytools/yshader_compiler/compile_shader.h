#ifndef YTOOLS_YSHADER_COMPILER_COMPILE_SHADER_H
#define YTOOLS_YSHADER_COMPILER_COMPILE_SHADER_H

#include <stdint.h>
#include <string>
#include <vector>

namespace ytools { namespace file_utils {
  class FileEnv;
}} // namespace ytools { namespace file_utils {

namespace ytools { namespace YShaderCompiler {

struct ShaderDefine {
  const char* define;
  const char* value;
};

enum ShaderOpt {
  kShaderOpt_None,
  kShaderOpt_Level0,
  kShaderOpt_Level1,
  kShaderOpt_Level2,
  kShaderOpt_Level3
};

enum ShaderType {
  kShaderType_VertexShader,
  kShaderType_PixelShader,

  NUM_SHADER_TYPES
};

bool CompileShader(const std::string& source_file,
                   const std::string& entry_point,
                   ShaderType shader_type, int target_major, int target_minor,
                   ShaderOpt opt_level, bool debug,
                   const ShaderDefine* defines, size_t num_defines,
                   std::vector<uint8_t>& output,
                   file_utils::FileEnv* file_env = NULL);

}} // namespace ytools { namespace YShaderCompiler {

#endif // YTOOLS_YSHADER_COMPILER_COMPILE_SHADER_H
