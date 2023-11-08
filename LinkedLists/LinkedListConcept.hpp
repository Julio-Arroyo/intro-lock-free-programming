#ifndef LINKED_LIST_CONCEPT_HPP
#define LINKED_LIST_CONCEPT_HPP

#include <concepts>
#include <memory>
#include <mutex>

template <typename T, typename E>
concept LinkedListConcept = requires (T t, E e) {
  { t.add(e)      } -> std::same_as<bool>;
  { t.remove(e)   } -> std::same_as<bool>;
  { t.contains(e) } -> std::same_as<bool>;
};

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

#endif

