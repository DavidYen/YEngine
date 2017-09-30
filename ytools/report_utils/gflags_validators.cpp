#include "ytools/report_utils/gflags_validators.h"

#include <iostream>

#include "ytools/file_utils/file_path.h"

namespace ytools { namespace report_utils {

bool GFlagsValidators::ValidateExistingFile(const char* flagname,
                                            const std::string& value) {
  if (!value.empty() && ytools::file_utils::FilePath::IsFile(value))
    return true;

  std::cerr << "Invalid value for --" << flagname << "." << std::endl;
  std::cerr << "  Expected valid file: " << value << std::endl;
  return false;
}

bool GFlagsValidators::ValidateFlagExists(const char* flagname,
                                          const std::string& value) {
  if (!value.empty())
    return true;

  std::cerr << "Flag --" << flagname << " is required." << std::endl;
  return false;
}

}} // namespace ytools { namespace report_utils {
