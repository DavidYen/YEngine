#ifndef YTOOLS_YSHADER_COMPILER_D3D_SHADER_COMPILER_H
#define YTOOLS_YSHADER_COMPILER_D3D_SHADER_COMPILER_H

#include "ytools/yshader_compiler/compile_shader.h"

namespace ytools { namespace YShaderCompiler {

namespace DXShaderCompiler {
  bool CompileShader(const std::string& source_file,
                     const std::string& entry_point,
                     const char* target, ShaderOpt opt_level, bool debug,
                     const ShaderDefine* defines, size_t num_defines,
                     std::vector<uint8_t>& output,
                     file_utils::FileEnv* file_env = NULL);
}

}} // namespace ytools { namespace YShaderCompiler {

#endif // YTOOLS_YSHADER_COMPILER_D3D_SHADER_COMPILER_H
