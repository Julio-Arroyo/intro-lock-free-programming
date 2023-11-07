#ifndef LINKED_LIST_CONCEPT_HPP
#define LINKED_LIST_CONCEPT_HPP

#include <concepts>

template <typename T>
concept LinkedListConcept = requires (T t) {
  { t.add()      } -> std::same_as<bool>;
  { t.remove()   } -> std::same_as<bool>;
  { t.contains() } -> std::same_as<bool>;
};

#endif

