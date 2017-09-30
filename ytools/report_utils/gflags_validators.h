#ifndef YTOOLS_REPORT_UTILS_GFLAGS_VALIDATORS_H
#define YTOOLS_REPORT_UTILS_GFLAGS_VALIDATORS_H

#include <string>

namespace ytools { namespace report_utils {

namespace GFlagsValidators {

  bool ValidateExistingFile(const char* flagname, const std::string& value);
  bool ValidateFlagExists(const char* flagname, const std::string& value);

} // namespace GFlagsValidators {

}} // namespace ytools { namespace report_utils {

#endif // YTOOLS_REPORT_UTILS_GFLAGS_VALIDATORS_H
