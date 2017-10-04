#include "ytools/ymodule_binary_compiler/asset_database.h"

#include <cstdio>
#include <iostream>

#include <rapidjson/document.h>

#include "ytools/file_utils/file_env.h"
#include "ytools/report_utils/json_errors.h"

namespace ytools { namespace ymodule_binary_compiler {

AssetDatabase::AssetDatabase(ytools::file_utils::FileEnv* file_env,
                             const std::string& file_path) {
  // Populate binary type string -> binary type map.
  std::unordered_map<std::string, yengine_data::BinaryType> binary_type_map;
  for (yengine_data::BinaryType i = yengine_data::BinaryType::MIN;
       i <= yengine_data::BinaryType::MAX;
       i = static_cast<yengine_data::BinaryType>(static_cast<int>(i) + 1)) {
    binary_type_map[yengine_data::EnumNameBinaryType(i)] = i;
  }

  rapidjson::Document doc;
  if (!report_utils::JsonErrors::ParseJsonFile(file_path.c_str(),
                                               doc,
                                               file_env)) {
    return;
  }

  if (!doc.IsObject()) {
    std::cerr << "Expected top level dictionary object:"
              << "\"" << file_path << "\"."
              << std::endl;
    return;
  }

  for (const auto& member : doc.GetObject()) {
    if (!member.name.IsString()) {
      std::cerr << "Asset Database expected string key value." << std::endl;
      return;
    }
    const char* member_name = member.name.GetString();

    if (!member.value.IsObject()) {
      std::cerr << "Asset \"" << member_name << "\" expected dict value."
                << std::endl;
      return;
    }

    const auto& member_dict = member.value.GetObject();

    const auto& asset_binary_type = member_dict["binary_type"];
    if (!asset_binary_type.IsString()) {
      std::cerr << "Asset \"" << member_name
                << "\" expected string binary_type value." << std::endl;
      return;
    }

    auto binary_type_iter = binary_type_map.find(asset_binary_type.GetString());
    if (binary_type_iter == binary_type_map.end()) {
      std::cerr << "Asset \"" << member_name
                << "\" has invalid binary_type value: "
                << asset_binary_type.GetString()
                << std::endl;
      return;
    }
    yengine_data::BinaryType binary_type_value = binary_type_iter->second;

    const auto& asset_file_path = member_dict["file_path"];
    if (!asset_file_path.IsString()) {
      std::cerr << "Asset \"" << member_name
                << "\" expected string file_path value." << std::endl;
      return;
    }

    mAssetDatabaseMap.insert(std::make_pair(
      std::string(member_name),
      AssetData({ binary_type_value, asset_file_path.GetString() })));
  }

  mValid = true;
}

bool AssetDatabase::GetAsset(const std::string& asset_name,
                             AssetData& data) const {
  const auto& iter = mAssetDatabaseMap.find(asset_name);
  if (iter == mAssetDatabaseMap.end()) {
    return false;
  }

  data = iter->second;
  return true;
}

}} // namespace ytools { namespace ymodule_binary_compiler {
