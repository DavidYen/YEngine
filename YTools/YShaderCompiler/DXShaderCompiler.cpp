#include <YCommon/Headers/stdincludes.h>
#include "DXShaderCompiler.h"

#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include <D3Dcompiler.h>

#include <YTools/YFileUtils/FileEnv.h>
#include <YTools/YFileUtils/FilePath.h>
#include <YTools/YFileUtils/FileStream.h>

namespace YTools { namespace YShaderCompiler {

namespace {
  std::unordered_map< std::string, std::vector<uint8_t> > mFileCache;
}

static const std::vector<uint8_t>* GetFileData(const std::string& abspath,
                                               YFileUtils::FileEnv* file_env) {
  auto map_iter = mFileCache.find(abspath);
  if (map_iter != mFileCache.end()) {
    return &(*map_iter).second;
  }

  // Read file into inserted vector.
  YFileUtils::FileStream filestream(abspath,
                                    YFileUtils::FileStream::kFileMode_Read,
                                    YFileUtils::FileStream::kFileType_Binary,
                                    file_env);
  if (!filestream.IsOpen()) {
    std::cerr << "Could not open file: " << abspath << std::endl;
    return NULL;
  }

  std::vector<uint8_t>& file_data = mFileCache[abspath];
  if (!filestream.Read(file_data)) {
    std::cerr << "Error reading file: " << abspath << std::endl;
    return NULL;
  }

  return &file_data;
}

class DXIncludeData : public ID3DInclude {
 public:
  DXIncludeData(const std::string& work_dir, YFileUtils::FileEnv* file_env)
      : mFileEnv(file_env) {
    mWorkDirStack.push({ NULL, work_dir });
  }

  HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName,
               LPCVOID pParentData, LPCVOID *ppData,
               UINT *pBytes) {
    (void) IncludeType;
    (void) pParentData;
    std::pair<const void*, std::string>& top = mWorkDirStack.top();
    std::string abspath = YFileUtils::FilePath::AbsPath(pFileName, top.second);

    const std::vector<uint8_t>* filedata = GetFileData(abspath, mFileEnv);
    if (filedata == NULL) {
      return E_FAIL;
    }

    // Push this directory on the top of the stack
    mWorkDirStack.push(std::make_pair<const void*, std::string>(
        static_cast<const void*>(filedata->data()),
                                 YFileUtils::FilePath::DirPath(abspath)));

    *ppData = filedata->data();
    *pBytes = static_cast<UINT>(filedata->size());
    return S_OK;
  }

  HRESULT Close(LPCVOID pData) {
    std::pair<const void*, std::string>& top = mWorkDirStack.top();
    (void) pData;
    (void) top;
    YASSERT(top.first == pData,
            "DXIncludeData Open/Close mismatch.");
    mWorkDirStack.pop();
    return S_OK;
  }

 private:
  YFileUtils::FileEnv* mFileEnv;
  std::stack< std::pair<const void*, std::string> > mWorkDirStack;
};

bool DXShaderCompiler::CompileShader(const std::string& source_file,
                                     const std::string& entry_point,
                                     const char* target,
                                     ShaderOpt opt_level, bool debug,
                                     const ShaderDefine* defines,
                                     size_t num_defines,
                                     std::vector<uint8_t>& output,
                                     YFileUtils::FileEnv* file_env) {
  std::string work_dir = YFileUtils::FilePath::GetCurrentWorkingDirectory();
  std::string abspath = YFileUtils::FilePath::AbsPath(source_file, work_dir);
  const std::vector<uint8_t>* source_data = GetFileData(abspath, file_env);

  std::vector<D3D_SHADER_MACRO> macros;
  for (size_t i = 0; i < num_defines; ++i) {
    macros.push_back({defines[i].define, defines[i].value});
  }
  macros.push_back({NULL, NULL});

  DXIncludeData include_data(work_dir, file_env);

  UINT flags1 = D3DCOMPILE_WARNINGS_ARE_ERRORS;
  switch (opt_level) {
   case kShaderOpt_None: flags1 |= D3DCOMPILE_SKIP_OPTIMIZATION; break;
   case kShaderOpt_Level0: flags1 |= D3DCOMPILE_OPTIMIZATION_LEVEL0; break;
   case kShaderOpt_Level1: flags1 |= D3DCOMPILE_OPTIMIZATION_LEVEL1; break;
   case kShaderOpt_Level2: flags1 |= D3DCOMPILE_OPTIMIZATION_LEVEL2; break;
   case kShaderOpt_Level3: flags1 |= D3DCOMPILE_OPTIMIZATION_LEVEL3; break;
   default:
     std::cerr << "Invalid Shader Opt: " << opt_level << std::endl;
     return false;
  }

  if (debug) {
    flags1 |= D3DCOMPILE_DEBUG;
  }

  ID3DBlob* code;
  ID3DBlob* error_msgs;

  HRESULT ret = D3DCompile2(
      source_data->data(), // in   LPCVOID pSrcData,
      source_data->size(), // in   SIZE_T SrcDataSize,
      source_file.c_str(), // in   LPCSTR pSourceName,
      macros.data(),       // in   const D3D_SHADER_MACRO *pDefines,
      &include_data,       // in   ID3DInclude *pInclude,
      entry_point.c_str(), // in   LPCSTR pEntrypoint,
      target,              // in   LPCSTR pTarget,
      flags1,              // in   UINT Flags1,
      0,                   // in   UINT Flags2,
      0,                   // in   UINT SecondaryDataFlags,
      NULL,                // in   LPCVOID pSecondaryData,
      0,                   // in   SIZE_T SecondaryDataSize,
      &code,               // out  ID3DBlob **ppCode,
      &error_msgs          // out  ID3DBlob **ppErrorMsgs
      );

  if (FAILED(ret)) {
    std::cerr << "[ERROR] "
              << static_cast<const char*>(error_msgs->GetBufferPointer())
              << std::endl;
    return false;
  }


  size_t code_size = code->GetBufferSize();
  output.resize(code_size);
  memcpy(output.data(), code->GetBufferPointer(), code_size);
  return true;
}

}} // namespace YTools { namespace YShaderCompiler {
