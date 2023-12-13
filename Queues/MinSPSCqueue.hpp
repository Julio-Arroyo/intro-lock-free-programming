#ifndef MIN_SPSC_QUEUE_HPP
#define MIN_SPSC_QUEUE_HPP

#define CAPACITY 1024

/* Queue stops working after exception is thrown */
class MinSPSCQueue {
public:
  MinSPSCQueue() {}

  /* Single-producer ==> only one thread is calling enqueue at a time
   *                 ==> only one thread is accessing tail at a time
   *
   * Reads from head
   * Updates tail
   */
  void enqueue(int v) {
    size_t front_pos = head.load(std::memory_order_relaxed);
    // only one thread can be enqueueing at a time
    size_t pos = tail.load(std::memory_order_relaxed);

    if (pos - front_pos >= CAPACITY) {
      throw std::runtime_error("enqueueing to full queue");
    }

    /* P1. Producer prepares data */
    ringBuffer[pos % CAPACITY] = v;

    /* P2. Producer publishes data */
    tail.store(pos + 1, std::memory_order_release);
  }

  /* Reads from tail
   * Updates head */
  int dequeue() {
    /* P3. Consumer receives published data */
    size_t tail_pos = tail.load(std::memory_order_acquire);

    size_t pos = head.fetch_add(1, std::memory_order_relaxed);  // incremented head becomes visible to producer
    if (pos == tail_pos)
      throw std::runtime_error("dequeueing from empty queue");
    }

    int tmp = ringBuffer[pos % CAPACITY];
    return tmp;
  }

private:
  std::atomic<size_t> head{0};
  std::atomic<size_t> tail{0};
  int ringBuffer[CAPACITY];
};

#endif

