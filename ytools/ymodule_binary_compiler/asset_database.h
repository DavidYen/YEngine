#ifndef YTOOLS_YMODULE_BINARY_COMPILER_ASSET_DATABASE_H
#define YTOOLS_YMODULE_BINARY_COMPILER_ASSET_DATABASE_H

#include <string>
#include <unordered_map>

#include <schemas/module_binary_types_generated.h>

namespace ytools { namespace file_utils {
  class FileEnv;
}} // namespace ytools { namespace file_utils {

namespace ytools { namespace ymodule_binary_compiler {

struct AssetData {
  yengine_data::BinaryType binary_type;
  std::string file_path;
};

class AssetDatabase {
 public:
  AssetDatabase(ytools::file_utils::FileEnv* file_env,
                const std::string& file_path);

  bool IsValid() const { return mValid; }

  bool GetAsset(const std::string& asset_name, AssetData& data) const;

 private:
  bool mValid = false;
  std::unordered_map<std::string, AssetData> mAssetDatabaseMap;
};

}} // namespace ytools { namespace ymodule_binary_compiler {

#endif // YTOOLS_YMODULE_BINARY_COMPILER_ASSET_DATABASE_H
