#ifndef COARSE_LIST_HPP
#define COARSE_LIST_HPP

#include <cassert>
#include <string>
#include <iostream>
#include "LinkedListConcept.hpp"

template <typename T>
class CoarseList {
public:
  CoarseList() : size{0} {
    head = std::make_shared<Node<T>>(T(), 0);
    tail = std::make_shared<Node<T>>(T(), std::numeric_limits<std::size_t>::max());
    head->next = tail;
    #ifdef ENABLE_LOGGING
    start = std::chrono::high_resolution_clock::now();
    #endif
  }

  CoarseList(const CoarseList& other) {
    assert(false);  // TODO: implement me
    // head = std::make_shared<Node<T>>(T(), 0);
    // tail = std::make_shared<Node<T>>(T(), std::numeric_limits<std::size_t>::max());
    // size = 0;

    // std::shared_ptr<Node<T>> curr = other->head->next;
    // while (curr->key < std::numeric_limits<std::size_t>::max()) {
    //   head
    // }
  }

  bool add(const T& val) {
    std::lock_guard<std::mutex> lock(mutex);

    std::size_t key = std::hash<T>{}(val);
    std::shared_ptr<Node<T>> pred = head;
    std::shared_ptr<Node<T>> curr = head->next;

    while (curr->key < key) {
      pred = curr;
      curr = curr->next;
    }

    bool isSuccessful = false;
    if (curr->key != key) {
      std::shared_ptr<Node<T>> node = std::make_shared<Node<T>>(val);
      node->next = curr;
      pred->next = node;
      isSuccessful = true;
    }

    #ifdef ENABLE_LOGGING
      std::chrono::time_point<std::chrono::high_resolution_clock>
        now = std::chrono::high_resolution_clock::now();
      std::chrono::microseconds
        time = std::chrono::duration_cast<std::chrono::microseconds>(now-start);
      std::printf("%x,ADD,%d,%d,%d,%d\n",
                  std::this_thread::get_id(),
                  key,
                  isSuccessful,
                  size,
                  time);
    #endif

    return isSuccessful;
  }
  
  bool remove(const T& val) {
    std::lock_guard<std::mutex> lock(mutex);

    std::size_t key = std::hash<T>{}(val);
    std::shared_ptr<Node<T>> pred = head;
    std::shared_ptr<Node<T>> curr = head->next;

    while (curr->key < key) {
      pred = curr;
      curr = curr->next;
    }

    bool isSuccessful = false;
    if (curr->key == key) {
      pred->next = curr->next;
      isSuccessful = true;
    }

    #ifdef ENABLE_LOGGING
      std::chrono::time_point<std::chrono::high_resolution_clock>
        now = std::chrono::high_resolution_clock::now();
      std::chrono::microseconds
        time = std::chrono::duration_cast<std::chrono::microseconds>(now-start);
      std::printf("%x,REM,%d,%d,%d,%d\n",
                  std::this_thread::get_id(),
                  key,
                  isSuccessful,
                  size,
                  time);
    #endif

    return isSuccessful;
  }

  bool contains(const T& val) {
    std::lock_guard<std::mutex> lock(mutex);

    std::size_t key =  std::hash<T>{}(val);
    std::shared_ptr<Node<T>> pred = head;
    std::shared_ptr<Node<T>> curr = head->next;

    while (curr->key < key) {
      pred = curr;
      curr = curr->next;
    }

    bool isSuccessful = false;
    if (curr->key == key) {
      isSuccessful = true;
    }

    #ifdef ENABLE_LOGGING
      std::chrono::time_point<std::chrono::high_resolution_clock>
        now = std::chrono::high_resolution_clock::now();
      std::chrono::microseconds
        time = std::chrono::duration_cast<std::chrono::microseconds>(now-start);
      std::printf("%x,CON,%d,%d,%d,%d\n",
                  std::this_thread::get_id(),
                  key,
                  isSuccessful,
                  size,
                  time);
      // print();
    #endif

    return isSuccessful;
  }

private:
  void print() {
    std::shared_ptr<Node<T>> curr = head->next;
    std::cout << "{";
    while (curr->key < tail->key) {
      std::cout << curr->key << ",";
      curr = curr->next;
    }
    std::cout << "}" << std::endl;
  }

  std::shared_ptr<Node<T>> head;
  std::shared_ptr<Node<T>> tail;
  std::size_t size;
  std::mutex mutex;  // global lock, acquired on every operation
  #ifdef ENABLE_LOGGING
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  #endif
};
static_assert(LinkedListConcept<CoarseList<std::string>, std::string>);
#endif

