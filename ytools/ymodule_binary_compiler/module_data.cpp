#include "ytools/ymodule_binary_compiler/module_data.h"

#include <cstdio>
#include <iostream>
#include <unordered_set>

#include <rapidjson/document.h>
#include <schemas/mesh_generated.h>
#include <schemas/module_binary_generated.h>
#include <schemas/render_targets_generated.h>
#include <schemas/sampler_generated.h>
#include <schemas/shader_generated.h>
#include <schemas/vertex_decl_generated.h>

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
    std::vector<uint8_t> arg_binary;
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

      case yengine_data::ModuleCommandType::RegisterVertexDecls:
        return ValidateRegisterVertDecls(command_args);

      case yengine_data::ModuleCommandType::RegisterSamplers:
        return ValidateRegisterSamplers(command_args);

      case yengine_data::ModuleCommandType::RegisterShaders:
        return ValidateRegisterShaders(command_args);

      case yengine_data::ModuleCommandType::RegisterMeshes:
        return ValidateRegisterMeshes(command_args);

      case yengine_data::ModuleCommandType::RegisterRenderTargets:
        return ValidateRegisterRenderTargets(command_args);

      default:
        std::cerr << "Unimplemented Command Type:" << command_type_name
                  << std::endl;
        return false;
      }
    }

   private:
    bool ValidateRegisterVertDecls(const std::vector<CommandArg>& command_args) {
      if (command_args.empty()) {
        std::cerr << "Register Vertex Declaration expected arguments."
                  << std::endl;
        return false;
      }
      for (const CommandArg& command_arg : command_args) {
        const std::string& decl_name = command_arg.arg_name;
        const ytools::ymodule_binary_compiler::AssetData& decl_asset =
          command_arg.arg_data;

        if (decl_asset.binary_type != yengine_data::BinaryType::kVertexDecl) {
          std::cerr << "Vertex Decl Registration expected kVertexDecl type, got "
                    << "\""
                    << yengine_data::EnumNameBinaryType(decl_asset.binary_type)
                    << "\": " << decl_name << std::endl;
          return false;
        }

        if (mVertexDecls.count(decl_name)) {
          std::cerr << "Vertex Declaration \"" << decl_name
                    << "\" has already been registered." << std::endl;
          return false;
        }

        // Validate the binary file
        flatbuffers::Verifier verifier(command_arg.arg_binary.data(),
                                       command_arg.arg_binary.size());
        if (!yengine_data::VerifyVertexDeclBuffer(verifier)) {
          std::cerr << "Invalid Vertex Declaration Binary: "
                    << command_arg.arg_data.file_path << std::endl;
          return false;
        }

        mVertexDecls.insert(decl_name);
      }
      return true;
    }

    bool ValidateRegisterSamplers(const std::vector<CommandArg>& command_args) {
      if (command_args.empty()) {
        std::cerr << "Register Samplers expected sampler arguments."
                  << std::endl;
        return false;
      }

      for (const CommandArg& command_arg : command_args) {
        const std::string& name = command_arg.arg_name;
        const ytools::ymodule_binary_compiler::AssetData& asset =
          command_arg.arg_data;

        if (asset.binary_type != yengine_data::BinaryType::kSampler) {
          std::cerr << "Sampler Registration expected kSampler type, got "
                    << "\""
                    << yengine_data::EnumNameBinaryType(asset.binary_type)
                    << "\": " << name << std::endl;
          return false;
        }

        if (mSamplers.count(name)) {
          std::cerr << "Sampler Declaration \"" << name
                    << "\" has already been registered." << std::endl;
          return false;
        }

        // Validate the binary file
        flatbuffers::Verifier verifier(command_arg.arg_binary.data(),
                                       command_arg.arg_binary.size());
        if (!yengine_data::VerifySamplerBuffer(verifier)) {
          std::cerr << "Invalid Sampler Binary: "
                    << command_arg.arg_data.file_path << std::endl;
          return false;
        }

        mSamplers.insert(name);
      }
      return true;
    }

    bool ValidateRegisterShaders(const std::vector<CommandArg>& command_args) {
      if (command_args.empty()) {
        std::cerr << "Register Shader command expected shader arguments."
                  << std::endl;
        return false;
      }
      for (const CommandArg& command_arg : command_args) {
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

        // Validate the binary file.
        flatbuffers::Verifier verifier(command_arg.arg_binary.data(),
                                       command_arg.arg_binary.size());
        if (!yengine_data::VerifyShaderBuffer(verifier)) {
          std::cerr << "Invalid Shader Binary: "
                    << command_arg.arg_data.file_path << std::endl;
          return false;
        }

        // Validate Shader Data
        const yengine_data::Shader* shader =
            yengine_data::GetShader(command_arg.arg_binary.data());

        if (shader->variants()->size() == 0) {
          std::cerr << "Shader \"" << shader_name << "\""
                    << " does not contain any shader variants."
                    << std::endl;
          return false;
        }

        for (const yengine_data::Variant* variant : *shader->variants()) {
          if (!mVertexDecls.count(variant->vertex_decl()->c_str())) {
            std::cerr << "Shader \"" << shader_name << "\""
                      << " uses unknown vertex declaration: "
                      << variant->vertex_decl()->c_str()
                      << std::endl;
            return false;
          }
        }

        mShaders.insert(shader_name);
      }

      return true;
    }

    bool ValidateRegisterMeshes(const std::vector<CommandArg>& command_args) {
      if (command_args.empty()) {
        std::cerr << "Register Mesh command expected mesh arguments."
                  << std::endl;
        return false;
      }
      for (const CommandArg& command_arg : command_args) {
        const std::string& mesh_name = command_arg.arg_name;
        const ytools::ymodule_binary_compiler::AssetData& mesh_asset =
            command_arg.arg_data;

        if (mesh_asset.binary_type != yengine_data::BinaryType::kMesh) {
          std::cerr << "Mesh Registration expected Mesh type, got \""
                    << yengine_data::EnumNameBinaryType(mesh_asset.binary_type)
                    << "\": " << mesh_name << std::endl;
          return false;
        }

        if (mMeshes.count(mesh_name)) {
          std::cerr << "Mesh \"" << mesh_name
                    << "\" has already been registered." << std::endl;
          return false;
        }

        // Validate the binary file.
        flatbuffers::Verifier verifier(command_arg.arg_binary.data(),
                                       command_arg.arg_binary.size());
        if (!yengine_data::VerifyMeshBuffer(verifier)) {
          std::cerr << "Invalid Mesh Binary: "
                    << command_arg.arg_data.file_path << std::endl;
          return false;
        }

        // Validate Mesh Data
        const yengine_data::Mesh* mesh =
            yengine_data::GetMesh(command_arg.arg_binary.data());

        mMeshes.insert(mesh->name()->c_str());
      }

      return true;
    }

    bool ValidateRegisterRenderTargets(
        const std::vector<CommandArg>& command_args) {
      if (command_args.empty()) {
        std::cerr << "Register Render Targets command expected render"
                  << " target arguments."
                  << std::endl;
        return false;
      }
      for (const CommandArg& command_arg : command_args) {
        const std::string& render_targets_name = command_arg.arg_name;
        const ytools::ymodule_binary_compiler::AssetData& render_targets_asset =
            command_arg.arg_data;

        if (render_targets_asset.binary_type !=
            yengine_data::BinaryType::kRenderTargets) {
          std::cerr << "Render Targets Registration got unexpected type \""
                    << yengine_data::EnumNameBinaryType(
                           render_targets_asset.binary_type)
                    << "\": " << render_targets_name << std::endl;
          return false;
        }

        if (mRenderTargets.count(render_targets_name)) {
          std::cerr << "Render Targets \"" << render_targets_name
                    << "\" has already been registered." << std::endl;
          return false;
        }

        // Validate the binary file.
        flatbuffers::Verifier verifier(command_arg.arg_binary.data(),
                                       command_arg.arg_binary.size());
        if (!yengine_data::VerifyRenderTargetsBuffer(verifier)) {
          std::cerr << "Invalid Render Targets Binary: "
                    << command_arg.arg_data.file_path << std::endl;
          return false;
        }

        // Validate Mesh Data
        const yengine_data::RenderTargets* render_targets =
            yengine_data::GetRenderTargets(command_arg.arg_binary.data());

        // Validate the render target definition.
        std::unordered_set<std::string> render_target_names;
        for (const auto& render_target : *render_targets->render_targets()) {
          // Make sure names are unique within the list of targets.
          std::string render_target_name(render_target->name()->c_str());
          if (render_target_names.count(render_target_name)) {
            std::cerr << "Render Targets \"" << render_target_name
                      << "\" declared multiple times." << std::endl;
            return false;
          }
          

          // Make sure the width/height percentages are valid.
          // 10x is arbitrary, but I don't see any reason why someone would need
          // a 10x width or height...
          if (render_target->width_percent() <= 0.0f ||
              render_target->width_percent() >= 10.0f) {
            std::cerr << "Render Target (" << render_target_name
                      << ") has an invalid width percentage: "
                      << render_target->width_percent() << std::endl;
            return false;
          }

          if (render_target->height_percent() <= 0.0f ||
              render_target->height_percent() >= 10.0f) {
            std::cerr << "Render Target (" << render_target_name
                      << ") has an invalid height percentage: "
                      << render_target->height_percent() << std::endl;
            return false;
          }

          // Make sure the render format is valid.
          if (render_target->render_format() ==
                  yengine_data::RenderFormat::kInvalid ||
              static_cast<int>(render_target->render_format()) < 
                  static_cast<int>(yengine_data::RenderFormat::MIN) ||
              static_cast<int>(render_target->render_format()) > 
                  static_cast<int>(yengine_data::RenderFormat::MAX)) {
            std::cerr << "Render Target (" << render_target_name
                      << ") has an invalid render format."
                      << std::endl;
            return false;
          }

          render_target_names.insert(std::move(render_target_name));
        }

        // Make sure render targets are actually defined.
        if (render_target_names.empty()) {
          std::cerr << "Render Targets ("
                    << render_targets->name()->c_str()
                    << ") has no render target definitions."
                    << std::endl;
          return false;
        }

        mRenderTargets.insert(render_targets->name()->c_str());
      }

      return true;
    }

    std::unordered_set<std::string> mVertexDecls;
    std::unordered_set<std::string> mSamplers;
    std::unordered_set<std::string> mShaders;
    std::unordered_set<std::string> mMeshes;
    std::unordered_set<std::string> mRenderTargets;
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

        file_utils::FileStream file(asset_data.file_path,
                                    file_utils::FileStream::kFileMode_Read,
                                    file_utils::FileStream::kFileType_Binary,
                                    file_env);

        if (!file.IsOpen()) {
          std::cerr << "Error loading asset \"" << arg_name << "\"."
                    << " Could not open file: "
                    << asset_data.file_path
                    << std::endl;
          return false;
        }

        std::vector<uint8_t> file_data;
        if (!file.Read(file_data)) {
          std::cerr << "Error loading asset \"" << arg_name << "\"."
                    << " Error reading file: "
                    << asset_data.file_path
                    << std::endl;
          return false;
        }

        command_args.push_back({ std::move(arg_name),
                                 std::move(asset_data),
                                 std::move(file_data) });
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

    // Create the module command data.
    ModuleCommand module_command_data;
    module_command_data.command_type = command_type;
    for (const CommandArg& command_arg : command_args) {
      module_command_data.command_args.push_back({
        command_arg.arg_data.binary_type,
        std::move(command_arg.arg_binary),
      });
    }

    mModuleCommands.push_back(std::move(module_command_data));
  }

  return true;
}

}} // namespace ytools { namespace ymodule_binary_compiler {
