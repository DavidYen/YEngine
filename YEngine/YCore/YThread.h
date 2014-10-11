#ifndef YFRAMEWORK_YTHREAD_H
#define YFRAMEWORK_YTHREAD_H

#include <string>

namespace YEngine { namespace YCore {

typedef int (*ThreadRoutine)(void* arg);

class YThread {
 public:
  YThread();
  YThread(ThreadRoutine thread_func, void* thread_arg, const char* name = NULL);
  ~YThread();

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
  char mPimpl[512-32];
  char mName[32];
};

}} // namespace YEngine { namespace YCore {

#endif // YFRAMEWORK_YTHREAD_H
