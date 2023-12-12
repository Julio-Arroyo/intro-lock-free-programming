#ifndef UNBOUNDED_QUEUE_HPP
#define UNBOUNDED_QUEUE_HPP

#include <mutex>
#include <memory>

template <typename T>
class Node {
public:
    Node(T& v) : val{v} {}

    T val;
    std::shared_ptr<T> next{nullptr};
};

template <typename T>
class UnboundedQueue {
public:
    void enqueue(T& v) {
        std::lock_guard<std::mutex> enqLockGuard(enqLock);
        std::shared_ptr<Node<T>> node = std::make_shared<Node<T>>(v);
        tail->next = node;
        tail = node;
    }

    std::shared_ptr<T> dequeue(T& v) {
        std::lock_guard<std::mutex> deqLockGuard(deqLock);

        if (head->next == nullptr) {
            return std::make_shared<T>(nullptr);
        }

        T res = head->next.val;
        head = head->next;
        return std::make_shared<T>(res);
    }

private:
    std::mutex enqLock;
    std::mutex deqLock;
    std::shared_ptr<Node<T>> head, tail;
};

#endif
