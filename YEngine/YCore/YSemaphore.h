#ifndef YCORE_YSEMAPHORE_H
#define YCORE_YSEMAPHORE_H

namespace YEngine { namespace YCore {

class YSemaphore {
 public:
  YSemaphore(int initial_count, int maximum_count);
  ~YSemaphore();

  bool Wait(size_t milliseconds = -1);
  bool Release();

 private:
  char mPimpl[32];
};

}} // namespace YEngine { namespace YCore {

#endif // YCORE_YSEMAPHORE_H
