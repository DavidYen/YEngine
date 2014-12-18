#ifndef YTOOLS_YSHADERCOMPILER_DXSHADERCOMPILER_H
#define YTOOLS_YSHADERCOMPILER_DXSHADERCOMPILER_H

#include "CompileShader.h"

namespace YTools { namespace YShaderCompiler {

namespace DXShaderCompiler {
  bool CompileShader(const std::string& source_file,
                     const std::string& entry_point,
                     const char* target, ShaderOpt opt_level, bool debug,
                     const ShaderDefine* defines, size_t num_defines,
                     std::vector<uint8_t>& output,
                     YFileUtils::FileEnv* file_env = NULL);
}

}} // namespace YTools { namespace YShaderCompiler {

#endif // YTOOLS_YSHADERCOMPILER_DXSHADERCOMPILER_H
