#include <iostream>

#include <crnlib.h>
#include <gflags/gflags.h>
#include <stb_image.h>
#include <schemas/texture_generated.h>

#include "ytools/file_utils/file_env.h"
#include "ytools/file_utils/file_path.h"
#include "ytools/file_utils/file_stream.h"
#include "ytools/report_utils/gflags_validators.h"
#include "ytools/report_utils/json_errors.h"

DEFINE_bool(verbose, false, "Verbose Output");
DEFINE_string(input_file, "", "Input texture json description file.");
DEFINE_string(output_file, "", "Output texture binary file.");
DEFINE_string(dep_file, "", "Dependency File.");

static const bool input_file_check = gflags::RegisterFlagValidator(
  &FLAGS_input_file,
  ytools::report_utils::GFlagsValidators::ValidateExistingFile);

static const bool output_file_check = gflags::RegisterFlagValidator(
  &FLAGS_output_file,
  ytools::report_utils::GFlagsValidators::ValidateFlagExists);

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  ytools::file_utils::FileEnv file_env;

  // Parse the JSON file.
  rapidjson::Document doc;
  if (!ytools::report_utils::JsonErrors::ParseJsonFile(FLAGS_input_file.c_str(),
                                                       doc,
                                                       &file_env)) {
    return 1;
  }
  if (!doc.IsObject()) {
    std::cerr << "Expected top level dict object: "
              << "\"" << FLAGS_input_file << "\"."
              << std::endl;
    return 1;
  }

  const rapidjson::Value::ConstMemberIterator name_iter =
      doc.FindMember("name");
  if (name_iter == doc.MemberEnd() || !name_iter->value.IsString()) {
    std::cerr << "Texture requires \"name\" string field." << std::endl;
    return 1;
  }
  const char* name_string = name_iter->value.GetString();

  const rapidjson::Value::ConstMemberIterator source_iter =
      doc.FindMember("source");
  if (source_iter == doc.MemberEnd() || !source_iter->value.IsString()) {
    std::cerr << "Texture requires \"source\" file string field." << std::endl;
    return 1;
  }
  const char* source_string = source_iter->value.GetString();

  const rapidjson::Value::ConstMemberIterator format_iter =
      doc.FindMember("format");
  if (format_iter == doc.MemberEnd() || !format_iter->value.IsString()) {
    std::cerr << "Texture requires \"format\" string field." << std::endl;
    return 1;
  }
  const char* format_string = format_iter->value.GetString();

  const std::string input_dir =
      ytools::file_utils::FilePath::DirPath(FLAGS_input_file);
  const std::string source_file =
      ytools::file_utils::FilePath::JoinPaths(input_dir, source_string);

  if (!ytools::file_utils::FilePath::IsFile(source_file)) {
    std::cerr << "Invalid Source Texture File: " << source_file << std::endl;
    return 1;
  }

  // Read the Source File.
  int width = 0;
  int height = 0;
  int channels = 0;
  file_env.AddReadFile(source_file);
  stbi_uc* stb_data = stbi_load(source_file.c_str(), &width, &height,
                                &channels, 0);
  if (!stb_data) {
    std::cerr << "Could not read source file: " << source_file << std::endl;
    return 1;
  }

  std::vector<uint8_t> source_data(width * height * 4);
  if (channels == 4) {
    source_data.insert(
      source_data.begin(),
      reinterpret_cast<uint8_t*>(stb_data),
      reinterpret_cast<uint8_t*>(stb_data) + (width * height * channels)
    );
  } else if (channels == 3) {
    // Fill in alpha channel.
    for (int i = 0; i < width * height; ++i) {
      source_data[i * 4] = stb_data[i * 3];
      source_data[i * 4 + 1] = stb_data[i * 3 + 1];
      source_data[i * 4 + 2] = stb_data[i * 3 + 2];
      source_data[i * 4 + 3] = 0xFF;
    }
  } else {
    std::cerr << "Expected at least 3 channels: " << source_file << std::endl;
    return 1;
  }
  stbi_image_free(stb_data);

  // Figure out the source format.
  std::vector<uint8_t> image_data;
  yengine_data::TextureFormat format = yengine_data::TextureFormat::kInvalid;
  if (0 == strcmp(format_string, "kDXT1")) {
    format = yengine_data::TextureFormat::kDXT1;
    crn_comp_params comp_params;
    comp_params.m_file_type = cCRNFileTypeDDS;
    comp_params.m_width = width;
    comp_params.m_height = height;
    comp_params.m_format = cCRNFmtDXT1;
    comp_params.m_pImages[0][0] =
        reinterpret_cast<const uint32_t*>(source_data.data());
    uint32_t comp_size = 0;
    void* comp_data = crn_compress(comp_params, comp_size);
    if (!comp_data) {
      std::cerr << "Could not compress image: " << source_file << std::endl;
      return 1;
    }

    image_data.insert(image_data.begin(),
                      static_cast<uint8_t*>(comp_data),
                      static_cast<uint8_t*>(comp_data) + comp_size);
    crn_free_block(comp_data);
  } else if (0 == strcmp(format_string, "kA8R8G8B8")) {
    format = yengine_data::TextureFormat::kA8R8G8B8;

    // Must swizzle alpha channel.
    image_data.resize(source_data.size());
    for (int i = 0; i < width * height; ++i) {
      image_data[i * 4] = source_data[i * 4 + 3];
      image_data[i * 4 + 1] = source_data[i * 4];
      image_data[i * 4 + 2] = source_data[i * 4 + 1];
      image_data[i * 4 + 3] = source_data[i * 4 + 2];
    }
  } else {
    std::cerr << "Unsupported texture format: " << format_string << std::endl;
    return 1;
  }

  // Start the Flatbuffer.
  flatbuffers::FlatBufferBuilder fbb;
  yengine_data::FinishTextureBuffer(
      fbb,
      yengine_data::CreateTexture(fbb,
                                  fbb.CreateString(name_string),
                                  format,
                                  fbb.CreateVector(image_data))
  );

  // Write out the file.
  ytools::file_utils::FileStream output_file(
      FLAGS_output_file,
      ytools::file_utils::FileStream::kFileMode_Write,
      ytools::file_utils::FileStream::kFileType_Binary,
      &file_env
  );

  if (!output_file.IsOpen()) {
    std::cerr << "Could not open output file: " << FLAGS_output_file
              << std::endl;
    return false;
  }

  if (!output_file.Write(fbb.GetBufferPointer(), fbb.GetSize())) {
    std::cerr << "Could not write output file: " << FLAGS_output_file
              << std::endl;
    return false;
  }

  // Write out the dependency file
  if (!FLAGS_dep_file.empty() &&
      !file_env.WriteDependencyFile(FLAGS_dep_file)) {
    return 1;
  }

  return 0;
}
