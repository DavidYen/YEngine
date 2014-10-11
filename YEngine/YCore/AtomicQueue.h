#ifndef YFRAMEWORK_ATOMICQUEUE_H
#define YFRAMEWORK_ATOMICQUEUE_H

/*******
* Atomic Queue with fixed size elements.
*  - Single producer (Only 1 thread can Enqueue at a time)
*  - Multiple consumers (Multiple threads can dequeue)
********/
namespace YEngine { namespace YCore {

class AtomicQueue {
 public:
  AtomicQueue(void* buffer, size_t item_size, size_t num_items);
  ~AtomicQueue();

  bool Enqueue(const void* data_item);
  bool Dequeue(void* data_item);

 private:
  char* mBuffer;
  size_t mItemSize;
  size_t mNumItems;

  volatile uint64_t mHead; // Point to first byte of queue
  volatile uint64_t mTail; // Points to last byte of queue (not the end)
};

}} // namespace YEngine { namespace YCore {

#endif // YFRAMEWORK_ATOMICQUEUE_H
