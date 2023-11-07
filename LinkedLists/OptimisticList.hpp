#ifndef OPTIMISTIC_LIST_HPP
#define OPTIMISTIC_LIST_HPP

#include "LinkedListConcept.hpp"

template <typename T>
class OptimisticList {
public:
  bool add() {
    return false;
  } 

  bool remove() {
    return false;
  }

  bool contains() {
    return false;
  }
};
static_assert(LinkedListConcept<OptimisticList<int>>);  // TODO: can 'int' be made arbitrary?

#endif

