
#include <iostream>

#include <gflags/gflags.h>
#include <schemas/bind_params_generated.h>

#include "ytools/file_utils/file_env.h"
#include "ytools/file_utils/file_stream.h"
#include "ytools/report_utils/gflags_validators.h"
#include "ytools/report_utils/json_errors.h"

DEFINE_bool(verbose, false, "Verbose Output");
DEFINE_string(input_file, "", "Input param data json description file.");
DEFINE_string(output_file, "", "Output param data binary file.");
DEFINE_string(dep_file, "", "Dependency File.");

static const bool input_file_check = gflags::RegisterFlagValidator(
  &FLAGS_input_file,
  ytools::report_utils::GFlagsValidators::ValidateExistingFile);

static const bool output_file_check = gflags::RegisterFlagValidator(
  &FLAGS_output_file,
  ytools::report_utils::GFlagsValidators::ValidateFlagExists);

struct DataType {
  const char* data_type;
  const char* data_primitive;
  size_t num_primitives;
  yengine_data::ParamType param_type;
};

static const DataType kDataTypes[] = {
  { "kFloat", "float", 1, yengine_data::ParamType::kFloat },
  { "kFloat2", "float", 2, yengine_data::ParamType::kFloat2 },
  { "kFloat3", "float", 3, yengine_data::ParamType::kFloat3 },
  { "kFloat4x4", "float", 16, yengine_data::ParamType::kFloat4x4 },
  { "kTexture", "texture", 1, yengine_data::ParamType::kTexture },
  { "kSampler", "sampler", 1, yengine_data::ParamType::kSampler },
};

bool ParseParam(const rapidjson::Value& param,
                flatbuffers::FlatBufferBuilder& fbb,
                flatbuffers::Offset<yengine_data::BindParam>& param_offset) {
  if (!param.IsObject()) {
    std::cerr << "Expected parameter dictionary object." << std::endl;
    return false;
  }

  const rapidjson::Value::ConstMemberIterator name_iter =
      param.FindMember("name");
  if (name_iter == param.MemberEnd() || !name_iter->value.IsString()) {
    std::cerr << "Param Data requires \"name\" string field." << std::endl;
    return false;
  }
  const char* name_string = name_iter->value.GetString();

  const rapidjson::Value::ConstMemberIterator type_iter =
      param.FindMember("type");
  if (type_iter == param.MemberEnd() || !type_iter->value.IsString()) {
    std::cerr << "Param Data (" << name_string << ")"
              << " requires \"type\" string field." << std::endl;
    return false;
  }
  const char* type_string = type_iter->value.GetString();

  yengine_data::ParamType param_type = yengine_data::ParamType::kInvalid;
  std::string param_ref;
  std::vector<uint8_t> param_data;

  for (const DataType& data_type : kDataTypes) {
    if (strcmp(data_type.data_type, type_string) == 0) {
      param_type = data_type.param_type;
      if (strcmp(data_type.data_primitive, "float") == 0) {
        const rapidjson::Value::ConstMemberIterator data_iter =
            param.FindMember("param_data");
        if (data_iter == param.MemberEnd()) {
          std::cerr << "Param Data (" << name_string << ")"
                    << " requires \"param_data\" field." << std::endl;
          return false;
        } else if (!data_iter->value.IsArray()) {
          std::cerr << "Param Data (" << name_string << ")"
                    << " required \"param_data\" field with array of floats."
                    << std::endl;
          return false;
        }

        std::vector<float> float_data;
        for (const rapidjson::Value& float_iter : data_iter->value.GetArray()) {
          if (!float_iter.IsFloat() && !float_iter.IsInt()) {
            std::cerr << "Param Data (" << name_string << ")"
                      << " expected \"param_data\" field with array of floats,"
                      << " received data type: "
                      << ytools::report_utils
                               ::JsonErrors
                               ::StringifyValueType(float_iter)
                      << std::endl;
            return false;
          }
          float_data.push_back(float_iter.GetFloat());
        }

        if (data_type.num_primitives != float_data.size()) {
          std::cerr << "Param Data (" << name_string << ")"
                      << " expected \"param_data\" field with "
                      << data_type.num_primitives
                      << " floats, received: "
                      << float_data.size()
                      << std::endl;
            return false;
        }

        const size_t float_size = sizeof(float_data[0]) * float_data.size();
        param_data.resize(float_size);
        memcpy(param_data.data(), float_data.data(), float_size);
      } else if (strcmp(data_type.data_primitive, "texture") == 0 ||
                 strcmp(data_type.data_primitive, "sampler") == 0) {
        const rapidjson::Value::ConstMemberIterator ref_iter =
            param.FindMember("param_ref");
        if (ref_iter == param.MemberEnd()) {
          std::cerr << "Param Data (" << name_string << ")"
                    << " requires \"param_ref\" field." << std::endl;
          return false;
        } else if (!ref_iter->value.IsString()) {
          std::cerr << "Param Data (" << name_string << ")"
                    << " required \"param_ref\" field with a param reference."
                    << std::endl;
          return false;
        }
        param_ref = ref_iter->value.GetString();
      } else {
        std::cerr << "Unimplemented Data Primitive: "
                  << data_type.data_primitive
                  << std::endl;
        return false;
      }

      break;
    }
  }

  if (param_type == yengine_data::ParamType::kInvalid) {
    std::cerr << "Unknown Param data type: " << type_string << std::endl;
    return false;
  }

  param_offset = yengine_data::CreateBindParam(
      fbb,
      fbb.CreateString(name_string),
      param_type,
      fbb.CreateString(param_ref.c_str()),
      fbb.CreateVector(param_data)
  );

  return true;
}

bool ParseParams(const rapidjson::Document& doc,
                 flatbuffers::FlatBufferBuilder& fbb) {
  if (!doc.IsObject()) {
    std::cerr << "Expected top level dictionary object." << std::endl;
    return false;
  }

  const rapidjson::Value::ConstMemberIterator name_iter =
      doc.FindMember("name");
  if (name_iter == doc.MemberEnd() || !name_iter->value.IsString()) {
    std::cerr << "Params Data table requires \"name\" string field."
              << std::endl;
    return false;
  }
  const char* name_string = name_iter->value.GetString();

  const rapidjson::Value::ConstMemberIterator params_iter =
      doc.FindMember("params");
  if (params_iter == doc.MemberEnd() || !params_iter->value.IsArray()) {
    std::cerr << "Params Data table (" << name_string << ")"
              << " requires \"params\" array field."
              << std::endl;
    return false;
  }

  std::vector<flatbuffers::Offset<yengine_data::BindParam> > bind_param_offsets;
  for (const rapidjson::Value& param_iter : params_iter->value.GetArray()) {
    flatbuffers::Offset<yengine_data::BindParam> param_offset;
    if (!ParseParam(param_iter, fbb, param_offset)) {
      return false;
    }
    bind_param_offsets.push_back(param_offset);
  }

  yengine_data::FinishBindParamsBuffer(
      fbb,
      yengine_data::CreateBindParams(
          fbb,
          fbb.CreateString(name_string),
          fbb.CreateVector(bind_param_offsets)
      )
  );

  return true;
}

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  ytools::file_utils::FileEnv file_env;

  rapidjson::Document doc;
  flatbuffers::FlatBufferBuilder fbb;
  if (!ytools::report_utils::JsonErrors::ParseJsonFile(FLAGS_input_file.c_str(),
                                                       doc,
                                                       &file_env) ||
      !ParseParams(doc, fbb)) {
    std::cerr << "Could not process param data file: "
              << FLAGS_input_file
              << std::endl;
    return 1;
  }

  // Write out the file.
  ytools::file_utils::FileStream output_file(
      FLAGS_output_file.c_str(),
      ytools::file_utils::FileStream::kFileMode_Write,
      ytools::file_utils::FileStream::kFileType_Binary,
      &file_env
  );

  if (!output_file.IsOpen()) {
    std::cerr << "Could not open output file: " << FLAGS_output_file
              << std::endl;
    return 1;
  }

  if (!output_file.Write(fbb.GetBufferPointer(), fbb.GetSize())) {
    std::cerr << "Could not write output file: " << FLAGS_output_file
              << std::endl;
    return 1;
  }

  // Write out the dependency file
  if (!FLAGS_dep_file.empty() &&
      !file_env.WriteDependencyFile(FLAGS_dep_file)) {
    return 1;
  }

  return 0;
}
