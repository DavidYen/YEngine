
#include <gflags/gflags.h>

#include "ytools/file_utils/file_env.h"
#include "ytools/report_utils/gflags_validators.h"

DEFINE_bool(verbose, false, "Verbose Output");
DEFINE_string(input_file, "", "Input material json description file.");
DEFINE_string(output_file, "", "Output material binary file.");
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

  // Write out the dependency file
  if (!FLAGS_dep_file.empty() &&
      !file_env.WriteDependencyFile(FLAGS_dep_file)) {
    return 1;
  }

  return 0;
}
