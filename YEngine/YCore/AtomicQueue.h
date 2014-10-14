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
  void* mBuffer;
  size_t mItemSize;
  size_t mNumItems;

  volatile uint64_t mHead; // Point to beginning of queue (dequeues from here)
  volatile uint64_t mTail; // Point to end of queue (enqueues here)
};

template<typename T>
class TypedAtomicQueue : public AtomicQueue {
 public:
  TypedAtomicQueue(T* buffer, size_t item_size, size_t num_items)
      : AtomicQueue(static_cast<void*>(buffer), item_size, num_items) {
  }

  bool Enqueue(const T* data_item) {
    return AtomicQueue::Enqueue(static_cast<const void*>(data_item));
  }

  bool Enqueue(const T& data_item) {
    return Enqueue(&data_item);
  }

  bool Dequeue(T* data_item) {
    return AtomicQueue::Dequeue(static_cast<void*>(data_item));
  }

  bool Dequeue(T& data_item) {
    return Dequeue(&data_item);
  }
};

}} // namespace YEngine { namespace YCore {

#endif // YFRAMEWORK_ATOMICQUEUE_H
