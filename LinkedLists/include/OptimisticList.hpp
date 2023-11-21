#ifndef OPTIMISTIC_LIST_HPP
#define OPTIMISTIC_LIST_HPP

#include <iostream>
#include <cassert>
#ifdef ENABLE_LOGGING
  #include <cstdio>
  #include <chrono>
#endif

#include "LinkedListConcept.hpp"

template <typename T>
class OptimisticList {
public:
  OptimisticList() {
    head = std::make_shared<Node<T>>(T(), 0);
    tail = std::make_shared<Node<T>>(T(), std::numeric_limits<std::size_t>::max());
    head->next = tail;
    size = 0;
    start = std::chrono::high_resolution_clock::now();
  }

  /* Returns true iff val was not present before adding it */
  bool add(const T& val) {
    std::size_t newKey = std::hash<T>{}(val);

    while (true) {  // loop repeatedly until validate() succeeds
      std::shared_ptr<Node<T>> pred = head;
      std::shared_ptr<Node<T>> curr = head->next;
      while (curr->key < newKey) {  // loop until pred->key < newKey <= curr->key
        pred = curr;
        curr = curr->next;
      }

      std::lock_guard<std::mutex> predLock(pred->mutex);
      std::lock_guard<std::mutex> currLock(curr->mutex);
      if (validate(pred, curr)) {
        bool isSuccessful = false;
        if (curr->key != newKey) {  // add newNode if not present
          std::shared_ptr<Node<T>> newNode = std::make_shared<Node<T>>(val);
          newNode->next = curr;
          pred->next = newNode;
          size++;
          isSuccessful = true;
        }

        #ifdef ENABLE_LOGGING
          std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
          std::chrono::microseconds time = std::chrono::duration_cast<std::chrono::microseconds>(now-start);
          std::printf("%10x,  ADD,%5d,%5s,%5d,%5d\n",
                      std::this_thread::get_id(),
                      newKey,
                      isSuccessful ? "true" : "false",
                      size,
                      time);
        #endif
        return isSuccessful;
      }
    }
    assert(false);  // unreachable
  } 

  /* Returns true if val was removed from the set, false if it was not present */
  bool remove(const T& val) {
    std::size_t removedKey = std::hash<T>{}(val);

    while (true) {
      std::shared_ptr<Node<T>> pred = head;
      std::shared_ptr<Node<T>> curr = head->next;
      while (curr->key < removedKey) {
        pred = curr;
        curr = curr->next;
      }

      std::lock_guard<std::mutex> predLock(pred->mutex);
      std::lock_guard<std::mutex> currLock(curr->mutex);
      // std::printf("pred: %d. curr: %d.\n", pred->key, curr->key);
      if (validate(pred, curr)) {
        bool isSuccessful = false;
        if (curr->key == removedKey) {
          pred->next = curr->next;
          size--;
          isSuccessful = true;
        }

        #ifdef ENABLE_LOGGING
          std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
          std::chrono::microseconds time = std::chrono::duration_cast<std::chrono::microseconds>(now-start);
          std::printf("%10x,  REM,%5d,%5s,%5d,%5d\n",
                      std::this_thread::get_id(),
                      removedKey,
                      isSuccessful ? "true" : "false",
                      size,
                      time);
        #endif
        return isSuccessful;
      }
    }
  }

  bool contains(const T& val) {
    std::size_t searchKey = std::hash<T>{}(val);

    // loop until pred, curr don't change
    while (true) {
      std::shared_ptr<Node<T>> pred = head;
      std::shared_ptr<Node<T>> curr = head->next;
      while (curr->key < searchKey) {  // iterate without locking
        pred = curr;
        curr = curr->next;
      }

      // pred, curr may be modified by another thread in this interval...

      std::lock_guard<std::mutex> predLock(pred->mutex);
      std::lock_guard<std::mutex> currLock(curr->mutex);

      // if pred, curr are unchanged perform operation.
      // Otherwise try again
      if (validate(pred, curr)) {  
        bool isSuccessful = curr->key == searchKey;
        #ifdef ENABLE_LOGGING
          std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
          std::chrono::microseconds time = std::chrono::duration_cast<std::chrono::microseconds>(now-start);
          std::printf("%x,CON,%d,%s,%d,%d\n",
                      std::this_thread::get_id(),
                      searchKey,
                      isSuccessful ? "true" : "false",
                      size,
                      time);
        #endif
        return curr->key == searchKey;
      }
    }
  }

  // void printLog() {
  //   #ifdef ENABLE_LOGGING
  //     // TODO add timestamps
  //     std::cout << "THREAD,OPERATION,VAL,RETURN" << std::endl;
  //     std::cout << log.str() << std::endl;
  //   #else
  //     std::cerr << "Logging is not enabled." << std::endl;
  //   #endif
  // }

private:
  /* Returns true if pred is reachable AND points to curr */
  bool validate(const std::shared_ptr<Node<T>>& pred, const std::shared_ptr<Node<T>>& curr) {
    std::shared_ptr<Node<T>> node = head;
    while (true) {
      if (node->key > pred->key) {
        return false;  // pred not found
      } else if (node->key == pred->key) {
        return pred->next == curr;
      }
      node = node->next;
    }
  }

  std::shared_ptr<Node<T>> head;
  std::shared_ptr<Node<T>> tail;
  std::size_t size;
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
};
static_assert(LinkedListConcept<OptimisticList<int>, int>);  // TODO: can 'int' be made arbitrary?

#endif

