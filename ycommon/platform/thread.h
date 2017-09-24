#ifndef YCOMMON_PLATFORM_THREAD_H
#define YCOMMON_PLATFORM_THREAD_H

#include <string>

#include "ycommon/headers/status_codes.h"

namespace ycommon { namespace platform {

typedef uintptr_t (*ThreadRoutine)(void* arg);

class Thread {
 public:
  Thread();
  Thread(ThreadRoutine thread_func, void* thread_arg, const char* name = NULL);
  ~Thread();

  bool Initialize(ThreadRoutine thread_func, void* thread_arg);
  YStatusCode Run();
  bool Join(size_t milliseconds = -1);

  void SetName(const char* name) {
    const size_t name_len = strlen(name);
    const size_t member_len = sizeof(mName) - 1;
    const size_t copy_len = member_len < name_len ? member_len : name_len;
    memcpy(mName, name, copy_len);
    mName[sizeof(mName)-1] = '\0';
  }

  const char* GetName() const {
    return mName;
  }

  bool IsRunning() const;

  // Returned undefined behavior if it is still running.
  uintptr_t ReturnValue() const;

 private:
  char mPimpl[128-32];
  char mName[32];
};

}} // namespace ycommon { namespace platform {

#endif // YCOMMON_PLATFORM_THREAD_H
