#ifndef YCOMMON_YPLATFORM_THREAD_H
#define YCOMMON_YPLATFORM_THREAD_H

#include <string>

namespace YCommon { namespace YPlatform {

typedef int (*ThreadRoutine)(void* arg);

class Thread {
 public:
  Thread();
  Thread(ThreadRoutine thread_func, void* thread_arg, const char* name = NULL);
  ~Thread();

  bool Initialize(ThreadRoutine thread_func, void* thread_arg);
  bool Run();
  bool join(size_t milliseconds = -1);

  void SetName(const char* name) {
    size_t name_len = strlen(name);
    memcpy(mName, name, std::min(sizeof(mName)-1, name_len));
    mName[sizeof(mName)-1] = '\0';
  }
  const char* GetName() const {
    return mName;
  }

  bool IsRunning() const;

  // Returned undefined behavior if it is still running.
  int ReturnValue() const;

 private:
  char mPimpl[128-32];
  char mName[32];
};

}} // namespace YCommon { namespace YPlatform {

#endif // YCOMMON_YPLATFORM_THREAD_H
