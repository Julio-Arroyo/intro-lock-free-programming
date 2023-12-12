#ifndef UNBOUNDED_LOCK_FREE_QUEUE_HPP
#define UNBOUNDED_LOCK_FREE_QUEUE_HPP

#include <atomic>
#include <memory>

template <typename T>
class Node {
public:
    Node (T& v) : val{v} {
        next.store(nullptr);
    }

    T val;
    std::atomic<std::shared_ptr<Node<T>> next;
};


template <typename T>
class LockFreeQueue {
public:
    LockFreeQueue() {
        head.store(std::make_shared<Node<T>(T()));  // sentinel
    }

    void enqueue(T& v) {
        std::shared_ptr<Node<T>> node = std::make_shared<Node<T>>(v);
        while (true) {
            std::shared_ptr<Node<T>> last = tail.load();
            std::shared_ptr<Node<T>> next = last->next.load();
            if (last == tail.load()) {
                if (next == nullptr) {
                    if (last->next.compare_exchange_strong(next, node)) {
                        tail.compare_exchange_strong(last, node);
                        return;
                    }
                } else {
                    tail.compare_exchange_strong(last, next);  // help with another thread's incomplete enqueue
                }
            }
        }
    }

    std::shared_ptr<T> dequeue() {
        while (true) {
            std::shared_ptr<Node<T>> first = head.load();
            std::shared_ptr<Node<T>> last = tail.load();
            std::shared_ptr<Node<T>> next = first->next.load();

            if (first == head.load()) {
                if (first == last) {
                    if (next == nullptr) {
                        return nullptr;
                    }
                    tail.compare_exchange_strong(last, next);
                }
            } else {
                std::shared_ptr<T> val = std::make_shared<T>(next.val);
                if (head.compare_exchange_strong(first, next)) {
                    return val;
                }
            }
        }
    }

private:
    std::atomic<std::shared_ptr<Node<T>> head, tail;
};

#endif
