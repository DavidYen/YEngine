#include "ytools/ymodule_binary_compiler/module_data.h"

#include <cstdio>
#include <iostream>
#include <unordered_set>

#include <rapidjson/document.h>
#include <schemas/module_binary_generated.h>

#include "ytools/file_utils/file_env.h"
#include "ytools/file_utils/file_stream.h"
#include "ytools/report_utils/json_errors.h"
#include "ytools/ymodule_binary_compiler/asset_database.h"

namespace {
  yengine_data::ModuleCommandType GetModuleCommandType(const char* type_name) {
    for (auto command_type : yengine_data::EnumValuesModuleCommandType()) {
      if (strcmp(type_name,
                 yengine_data::EnumNameModuleCommandType(command_type)) == 0) {
        return command_type;
      }
    }

    return yengine_data::ModuleCommandType::kInvalidModuleCommandType;
  }

  struct CommandArg {
    std::string arg_name;
    ytools::ymodule_binary_compiler::AssetData arg_data;
  };

  // The ModuleCommandValidation class runs through each command in order
  // and does some basic checks such as making sure the commands are being
  // registered in the proper order.
  class ModuleCommandValidation {
   public:
    ModuleCommandValidation() = default;

    bool ValidateCommand(const char* command_type_name,
                         yengine_data::ModuleCommandType command_type,
                         const std::vector<CommandArg>& command_args) {
      switch (command_type) {
      case yengine_data::ModuleCommandType::kInvalidModuleCommandType:
        std::cerr << "Invalid Module Command Type: " << command_type_name
                  << std::endl;
        return false;
      case yengine_data::ModuleCommandType::RegisterShader:
        return ValidateRegisterShader(command_args);
      default:
        std::cerr << "Unimplemented Command Type:" << command_type_name
                  << std::endl;
        return false;
      }
    }

   private:
    bool ValidateRegisterShader(const std::vector<CommandArg>& command_args) {
      if (command_args.size() != 1) {
        std::cerr << "Register Shader command expected 1 shader argument."
                  << std::endl;
        return false;
      }
      const CommandArg& command_arg = command_args.front();
      const std::string& shader_name = command_arg.arg_name;
      const ytools::ymodule_binary_compiler::AssetData& shader_asset =
        command_arg.arg_data;

      if (shader_asset.binary_type != yengine_data::BinaryType::kShader) {
        std::cerr << "Shader Registration expected Shader type, got \""
                  << yengine_data::EnumNameBinaryType(shader_asset.binary_type)
                  << "\": " << shader_name << std::endl;
        return false;
      }

      if (mShaders.count(shader_name)) {
        std::cerr << "Shader \"" << shader_name
                  << "\" has already been registered." << std::endl;
        return false;
      }
      mShaders.insert(shader_name);
      return true;
    }

    std::unordered_set<std::string> mShaders;
  };
}

namespace ytools { namespace ymodule_binary_compiler {

ModuleData::ModuleData(ytools::file_utils::FileEnv* file_env,
                       const AssetDatabase& asset_database,
                       const std::string& file_path) {
  mValid = ProcessModuleData(file_env, asset_database, file_path);
}

bool ModuleData::WriteModuleDataFile(ytools::file_utils::FileEnv* file_env,
                                     const std::string& file_path) {
  flatbuffers::FlatBufferBuilder fbb;

  std::vector<flatbuffers::Offset<yengine_data::ModuleCommand> > fbb_commands;
  for (const ModuleCommand& module_command : mModuleCommands) {
    std::vector<flatbuffers::Offset<yengine_data::BinaryData> > fbb_args;
    for (const BinaryData& arg_data : module_command.command_args) {
      fbb_args.push_back(yengine_data::CreateBinaryData(
          fbb, arg_data.binary_type, fbb.CreateVector(arg_data.data)));
    }

    fbb_commands.push_back(yengine_data::CreateModuleCommand(
        fbb, module_command.command_type, fbb.CreateVector(fbb_args)));
  }

  auto module_binary_loc = yengine_data::CreateModuleBinary(
      fbb, fbb.CreateString(mName), fbb.CreateVector(fbb_commands)
  );

  yengine_data::FinishModuleBinaryBuffer(fbb, module_binary_loc);

  // Write out the file.
  ytools::file_utils::FileStream output_file(
      file_path,
      ytools::file_utils::FileStream::kFileMode_Write,
      ytools::file_utils::FileStream::kFileType_Binary,
      file_env
  );

  if (!output_file.IsOpen()) {
    std::cerr << "Could not open output file: " << file_path << std::endl;
    return false;
  }

  if (!output_file.Write(fbb.GetBufferPointer(), fbb.GetSize())) {
    std::cerr << "Could not write output file: " << file_path << std::endl;
    return false;
  }

  return true;
}

bool ModuleData::ProcessModuleData(ytools::file_utils::FileEnv* file_env,
                                   const AssetDatabase& asset_database,
                                   const std::string& file_path) {
  ModuleCommandValidation validator;

  rapidjson::Document doc;
  if (!report_utils::JsonErrors::ParseJsonFile(file_path.c_str(),
                                               doc,
                                               file_env)) {
    return false;
  }

  if (!doc.IsObject()) {
    std::cerr << "Expected top level dictionary object:"
              << "\"" << file_path << "\"."
              << std::endl;
    return false;
  }

  // Parse the name.
  const auto& module_name = doc["name"];
  if (!module_name.IsString()) {
    std::cerr << "Module description expected string \"name\" field:"
              << "\"" << file_path << "\"."
              << std::endl;
    return false;
  }
  mName = module_name.GetString();

  // Parse the commands.
  const auto& module_commands = doc["module_commands"];
  if (!module_commands.IsArray()) {
    std::cerr << "Module \"" << mName
              << "\" expected array \"module_commands\" field."
              << std::endl;
    return false;
  }

  for (const auto& module_command : module_commands.GetArray()) {
    if (!module_command.IsObject()) {
      std::cerr << "Module \"" << mName
                << "\" has commands which are not dicts."
                << std::endl;
      return false;
    }

    const auto& module_command_type = module_command["command_type"];
    if (!module_command_type.IsString()) {
      std::cerr << "Module \"" << mName
                << "\" has a command with no \"command_type\" field."
                << std::endl;
      return false;
    }
    const char* command_type_string = module_command_type.GetString();

    const auto& module_command_args = module_command["command_args"];
    if (!module_command_args.IsArray() && !module_command_args.IsNull()) {
      std::cerr << "Module \"" << mName
                << "\" has \"" << command_type_string
                << "\" command with an invalid \"command_args\" field."
                << std::endl;
      return false;
    }

    std::vector<CommandArg> command_args;
    if (module_command_args.IsArray()) {
      for (const auto& module_command_arg : module_command_args.GetArray()) {
        if (!module_command_arg.IsString()) {
          std::cerr << "Module \"" << mName
                    << "\" has \"" << command_type_string
                    << "\"commands expects only string command_args."
                    << std::endl;
          return false;
        }

        std::string arg_name(module_command_arg.GetString());
        ytools::ymodule_binary_compiler::AssetData asset_data;
        if (!asset_database.GetAsset(arg_name, asset_data)) {
          std::cerr << "Module \"" << mName
                    << "\" referenced unknown asset data: "
                    << arg_name
                    << std::endl;
          return false;
        }

        command_args.push_back({ std::move(arg_name), std::move(asset_data) });
      }
    }

    const yengine_data::ModuleCommandType command_type =
      GetModuleCommandType(command_type_string);

    // Run the validator
    if (!validator.ValidateCommand(command_type_string,
                                   command_type,
                                   command_args)) {
      return false;
    }

    // Everything checks out, create the module command data.
    ModuleCommand module_command_data;
    module_command_data.command_type = command_type;
    for (const CommandArg& command_arg : command_args) {
      BinaryData binary_data;
      binary_data.binary_type = command_arg.arg_data.binary_type;
      file_utils::FileStream file(command_arg.arg_data.file_path,
                                  file_utils::FileStream::kFileMode_Read,
                                  file_utils::FileStream::kFileType_Binary,
                                  file_env);

      if (!file.IsOpen()) {
        std::cerr << "Error loading asset \"" << command_arg.arg_name << "\"."
                  << " Could not open file: "
                  << command_arg.arg_data.file_path
                  << std::endl;
        return false;
      }

      if (!file.Read(binary_data.data)) {
        std::cerr << "Error loading asset \"" << command_arg.arg_name << "\"."
                  << " Error reading file: "
                  << command_arg.arg_data.file_path
                  << std::endl;
        return false;
      }
      module_command_data.command_args.push_back(std::move(binary_data));
    }

    mModuleCommands.push_back(std::move(module_command_data));
  }

  return true;
}

}} // namespace ytools { namespace ymodule_binary_compiler {
