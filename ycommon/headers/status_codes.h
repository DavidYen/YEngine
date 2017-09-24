#ifndef YCOMMON_HEADERS_STATUSCODES_H
#define YCOMMON_HEADERS_STATUSCODES_H

enum YStatusCode {
  kStatusCode_OK,
  kStatusCode_ERROR,

  kStatusCode_InvalidArguments,
  kStatusCode_SystemError,
  kStatusCode_AlreadyRunning,

  NUM_STATUS_CODES
};

static const char* kStatusCodeStrings[] = {
  "OK",                   // kStatusCode_OK,
  "ERROR",                // kStatusCode_ERROR,
  "Invalid Arguments",    // kStatusCode_InvalidArguments,
  "System Error",         // kStatusCode_SystemError,
  "Already Running",      //kStatusCode_AlreadyRunning,
};
static_assert(sizeof(kStatusCodeStrings) / sizeof(const char*) ==
              NUM_STATUS_CODES,
              "Status Code strings must match all status codes.");


#endif // YCOMMON_HEADERS_STATUSCODES_H
