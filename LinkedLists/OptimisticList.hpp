#ifndef OPTIMISTIC_LIST_HPP
#define OPTIMISTIC_LIST_HPP

#include <iostream>
#include <cassert>
#include "LinkedListConcept.hpp"

template <typename T>
class OptimisticList {
public:
  OptimisticList() {
    head = std::make_shared<Node<T>>(T(), 0);
    tail = std::make_shared<Node<T>>(T(), std::numeric_limits<std::size_t>::max());
    head->next = tail;
    size = 0;
  }

  /* Returns true iff val was not present before adding it */
  bool add(const T& val) {
    std::shared_ptr<Node<T>> pred = head;
    std::shared_ptr<Node<T>> curr = head->next;
    std::size_t newKey = std::hash<T>{}(val);

    while (true) {  // loop repeatedly until validate() succeeds
      while (curr->key < newKey) {  // loop until pred->key < newKey <= curr->key
        pred = curr;
        curr = curr->next;
      }

      std::lock_guard<std::mutex> predLock(pred->mutex);
      std::lock_guard<std::mutex> currLock(curr->mutex);
      if (validate(pred, curr)) {
        if (curr->key != newKey) {  // add newNode if not present
          Node dn = Node(val);
          std::shared_ptr<Node<T>> newNode = std::make_shared<Node<T>>(val);
          newNode->next = curr;
          pred->next = newNode;
          size++;

          #ifdef DEBUG_LOGGING
            std::cout << "ADD," << val << "," << true << std::endl;
          #endif
          return true;
        }

        #ifdef DEBUG_LOGGING
          std::cout << "ADD," << val << "," << false << std::endl;
        #endif
        return false;
      }
    }
    assert(false);  // unreachable
  } 

  /* Returns true if val was removed from the set, false if it was not present */
  bool remove(const T& val) {
    std::shared_ptr<Node<T>> pred = head;
    std::shared_ptr<Node<T>> curr = head->next;
    std::size_t removedKey = std::hash<T>{}(val);

    while (true) {
      while (curr->key < removedKey) {
        pred = curr;
        curr = curr->next;
      }

      std::lock_guard<std::mutex> predLock(pred->mutex);
      std::lock_guard<std::mutex> currLock(curr->mutex);
      if (validate(pred, curr)) {
        if (curr->key == removedKey) {
          pred->next = curr->next;
          size--;

          #ifdef DEBUG_LOGGING
            std::cout << "REM," << val << "," << true << std::endl;
          #endif
          return true;
        } else {
          #ifdef DEBUG_LOGGING
            std::cout << "REM," << val << "," << false << std::endl;
          #endif
          return false;
        }
      }
    }
  }

  bool contains(const T& val) {
    std::shared_ptr<Node<T>> pred = head;
    std::shared_ptr<Node<T>> curr = head->next;
    std::size_t searchKey = std::hash<T>{}(val);

    while (true) {
      while (curr->key < searchKey) {
        pred = curr;
        curr = curr->next;
      }

      std::lock_guard<std::mutex> predLock(pred->mutex);
      std::lock_guard<std::mutex> currLock(curr->mutex);
      if (validate(pred, curr)) {
        #ifdef DEBUG_LOGGING
          std::cout << "CON," << val << "," << (curr->key == searchKey) << std::endl;
        #endif
        return curr->key == searchKey;
      }
    }
  }

private:
  /* Returns true if pred is reachable AND points to curr */
  bool validate(const std::shared_ptr<Node<T>> pred, const std::shared_ptr<Node<T>> curr) {
    std::shared_ptr<Node<T>> node = head;
    while (true) {
      if (node->key > pred->key) {
        return false;  // pred not found
      } else if (node->key == pred->key) {
        return pred->next->key = curr->key;
      }
      node = node->next;
    }
  }

  std::shared_ptr<Node<T>> head;
  std::shared_ptr<Node<T>> tail;
  std::size_t size;
};
static_assert(LinkedListConcept<OptimisticList<int>, int>);  // TODO: can 'int' be made arbitrary?

#endif

