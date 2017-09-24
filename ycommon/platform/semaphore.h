#ifndef YCOMMON_PLATFORM_SEMAPHORE_H
#define YCOMMON_PLATFORM_SEMAPHORE_H
namespace ycommon { namespace platform {

class Semaphore {
 public:
  Semaphore();
  Semaphore(int initial_count, int maximum_count);
  ~Semaphore();

  void Initialize(int initial_count, int maximum_count);
  bool Wait(size_t milliseconds = -1);
  bool Release(int count = 1);

 private:
  char mPimpl[32];
};

}} // namespace ycommon { namespace platform {

#endif // YCOMMON_PLATFORM_SEMAPHORE_H
