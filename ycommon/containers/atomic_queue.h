#ifndef YCOMMON_CONTAINERS_ATOMIC_QUEUE_H
#define YCOMMON_CONTAINERS_ATOMIC_QUEUE_H

#include <stdint.h>

/*******
* Atomic Queue with fixed size elements.
*  - Single producer (Only 1 thread can Enqueue at a time)
*  - Multiple consumers (Multiple threads can dequeue)
********/
namespace ycommon { namespace containers {

class AtomicQueue {
 public:
  AtomicQueue();
  AtomicQueue(void* buffer, size_t buffer_size,
              size_t item_size, size_t num_items);
  ~AtomicQueue();

  void Initialize(void* buffer, size_t buffer_size,
                  size_t item_size, size_t num_items);
  bool Enqueue(const void* data_item);
  bool Dequeue(void* data_item);
  size_t CurrentSize() const;

  size_t ItemSize() const { return mItemSize; }
  size_t Size() const { return mNumItems; }

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
  TypedAtomicQueue() : AtomicQueue() {}
  TypedAtomicQueue(T* buffer, size_t buffer_size, size_t num_items)
      : AtomicQueue(static_cast<void*>(buffer), buffer_size,
                    sizeof(T), num_items) {}

  void Initialize(T* buffer, size_t buffer_size, size_t num_items) {
    AtomicQueue::Initialize(static_cast<void*>(buffer), buffer_size,
                            sizeof(T), num_items);
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

template<typename T, size_t items>
class ContainedAtomicQueue : public TypedAtomicQueue<T> {
 public:
  ContainedAtomicQueue() : TypedAtomicQueue(mData, sizeof(mData), items) {}
  ~ContainedAtomicQueue() {}

 private:
  T mData[items];
};

}} // namespace ycommon { namespace containers {

#endif // YCOMMON_CONTAINERS_ATOMIC_QUEUE_H
