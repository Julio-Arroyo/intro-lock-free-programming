#ifndef LINKED_LIST_CONCEPT_HPP
#define LINKED_LIST_CONCEPT_HPP

#include <concepts>
#include <memory>
#include <mutex>
#include <atomic>
#include "AtomicMarkableReference.hpp"

template <typename T, typename E>
concept LinkedListConcept = requires (T t, E e) {
  { t.add(e)      } -> std::same_as<bool>;
  { t.remove(e)   } -> std::same_as<bool>;
  { t.contains(e) } -> std::same_as<bool>;
};

enum ModificationType {
  UNKNOWN,
  ADD,
  REMOVE
};

// TODO: move Node classes to a new file Node.hpp
template <typename T>
class Node {
public:
  Node(const T& v) : val{v}, key{std::hash<T>{}(v)} { }

  // to create sentinels
  Node(const T& v, const std::size_t k) : val{v}, key{k} { }

  std::shared_ptr<Node<T>> next;
  std::mutex mutex;
  std::size_t key;

private:
  T val;
};

template <typename T>
class MarkedNode {
public:
  MarkedNode(const T& v) : val{v}, key{std::hash<T>{}(v)} { }

  // to create sentinels
  MarkedNode(const T& v, const std::size_t k) : val{v}, key{k} { }

  std::shared_ptr<MarkedNode<T>> next{nullptr};
  std::mutex mutex;
  std::size_t key{0};
  bool removed{false};
  T val;
};

// Node class for LockFreeList
template <typename T>
class LockFreeNode {
public:
  LockFreeNode(const T& v) : val{v}, key{std::hash<T>{}(v)} { }
  LockFreeNode(const T& v, const std::size_t k) : val{v}, key{k} { }

  /* Returns true if THIS node is removed. Note that the flag is stored
   * in the AtomicMarkableReference */
  bool isRemoved() {
    return next.isMarked();
  }

  std::shared_ptr<LockFreeNode<T>> getNext() {
    return false;
  }

  void setNext(std::shared_ptr<T> succ) {
    return false;
  }

  bool attemptMarkAsRemoved() {
    assert(false);
    return false;
  }

  AtomicMarkableReference<LockFreeNode<T>> next{nullptr, false};
  std::size_t key{0};
  T val{};
};

#endif

