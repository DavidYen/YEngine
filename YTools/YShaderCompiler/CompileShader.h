#ifndef YTOOLS_YSHADERCOMPILER_COMPILESHADER_H
#define YTOOLS_YSHADERCOMPILER_COMPILESHADER_H

#include <string>
#include <vector>

namespace YTools { namespace YFileUtils {
  class FileEnv;
}} // namespace YTools { namespace YFileUtils {

namespace YTools { namespace YShaderCompiler {

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
                   YFileUtils::FileEnv* file_env = NULL);

}} // namespace YTools { namespace YShaderCompiler {

#endif // YTOOLS_YSHADERCOMPILER_COMPILESHADER_H
