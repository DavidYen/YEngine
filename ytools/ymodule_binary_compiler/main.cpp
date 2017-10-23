
#include <gflags/gflags.h>

#include "ytools/file_utils/file_env.h"
#include "ytools/file_utils/file_path.h"
#include "ytools/file_utils/file_stream.h"
#include "ytools/report_utils/gflags_validators.h"
#include "ytools/ymodule_binary_compiler/asset_database.h"
#include "ytools/ymodule_binary_compiler/module_data.h"

DEFINE_bool(verbose, false, "Verbose Output");
DEFINE_string(database_file, "", "Asset Database which contains all assets.");
DEFINE_string(input_file, "", "Input module binary data json file.");
DEFINE_string(output_file, "", "Output module binary file.");
DEFINE_string(dep_file, "", "Dependency File.");

static const bool database_file_check = gflags::RegisterFlagValidator(
  &FLAGS_database_file,
  ytools::report_utils::GFlagsValidators::ValidateExistingFile);

static const bool input_file_check = gflags::RegisterFlagValidator(
  &FLAGS_input_file,
  ytools::report_utils::GFlagsValidators::ValidateExistingFile);

static const bool output_file_check = gflags::RegisterFlagValidator(
  &FLAGS_output_file,
  ytools::report_utils::GFlagsValidators::ValidateFlagExists);

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  ytools::file_utils::FileEnv file_env;

  // Parse the Asset Database
  ytools::ymodule_binary_compiler::AssetDatabase asset_db(&file_env,
                                                          FLAGS_database_file);
  if (!asset_db.IsValid()) {
    return 1;
  }

  // Construct the module binary data
  ytools::ymodule_binary_compiler::ModuleData module_data(&file_env,
                                                          asset_db,
                                                          FLAGS_input_file);
  if (!module_data.IsValid()) {
    return 1;
  }

  // Write out the module binary file
  if (!module_data.WriteModuleDataFile(&file_env, FLAGS_output_file)) {
    return 1;
  }

  // Write out the dependency file
  if (!FLAGS_dep_file.empty() &&
      !file_env.WriteDependencyFile(FLAGS_dep_file)) {
    return 1;
  }

  return 0;
}
