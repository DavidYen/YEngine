#ifndef YCOMMON_YPLATFORM_SEMAPHORE_H
#define YCOMMON_YPLATFORM_SEMAPHORE_H

namespace YCommon { namespace YPlatform {

class Semaphore {
 public:
  Semaphore(int initial_count, int maximum_count);
  ~Semaphore();

  bool Wait(size_t milliseconds = -1);
  bool Release();

 private:
  char mPimpl[32];
};

}} // namespace YCommon { namespace YPlatform {

#endif // YCOMMON_YPLATFORM_SEMAPHORE_H
