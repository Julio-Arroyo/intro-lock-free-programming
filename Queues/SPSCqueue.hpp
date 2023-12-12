#include <memory>  // unique_ptr
#include <atomic>

template <typename T>
class SPSCqueue
{
public:
    SPSCqueue(int capacity)
    : capacity_{capacity},
      head_{0},
      tail_{0}
    {
        ringBuffer_ = std::make_unique<T[]>(capacity);
    }

    void enqueue(T v) {
        if (tail_.load(std::memory_order_seq_cst) - head.load(std::memory_order_seq_cst) == capacity_) {
            throw std::exception("FullException");
        }
        int tail = tail_.load(std::memory_order_seq_cst);
        ringBuffer_[tail] = v;
        tail_.store((tail+1) % capacity_, std::memory_order_seq_cst);
        return;
    }

    T dequeue() {
        if (tail_.load(std::memory_order_seq_cst) == head.load(std::memory_order_seq_cst)) {
            throw std::exception("EmptyException");
        }

        int head = head_.load(std::memory_order_seq_cst);
        T v = ringBuffer_[head];
        head_.store((head+1) % capacity_, std::memory_order_seq_cst);  // TODO: use relaxed, release, acquire instead
        return v;
    }

private:
    int capacity_;
    std::atomic_int head_;
    std::atomic_int tail_;
    std::unique_ptr<T[]> ringBuffer_;
};
