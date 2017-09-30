#ifndef YTOOLS_YMODULE_BINARY_COMPILER_MODULE_DATA_H
#define YTOOLS_YMODULE_BINARY_COMPILER_MODULE_DATA_H

#include <string>
#include <vector>

#include <schemas/module_binary_generated.h>

namespace ytools { namespace file_utils {
  class FileEnv;
}} // namespace ytools { namespace file_utils {

namespace ytools { namespace ymodule_binary_compiler {

class AssetDatabase;

class ModuleData {
 public:
  ModuleData(ytools::file_utils::FileEnv* file_env,
             const AssetDatabase& asset_database,
             const std::string& file_path);

  bool IsValid() { return mValid; }
  bool WriteModuleDataFile(ytools::file_utils::FileEnv* file_env,
                           const std::string& file_path);

 private:
  bool ProcessModuleData(ytools::file_utils::FileEnv* file_env,
                         const AssetDatabase& asset_database,
                         const std::string& file_path);

  struct BinaryData {
    yengine_data::BinaryType binary_type;
    std::vector<uint8_t> data;
  };

  struct ModuleCommand {
    yengine_data::ModuleCommandType command_type;
    std::vector<BinaryData> command_args;
  };

  bool mValid = false;

  std::string mName;
  std::vector<ModuleCommand> mModuleCommands;
};

}} // namespace ytools { namespace ymodule_binary_compiler {

#endif // YTOOLS_YMODULE_BINARY_COMPILER_MODULE_DATA_H
