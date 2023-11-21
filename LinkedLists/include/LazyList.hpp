#ifndef LAZY_LIST_HPP
#define LAZY_LIST_HPP

#include <cassert>
#include <functional>  // std::hash
#include <string>
#include <limits>  // std::numeric_limits
#include <cstdlib>  // std::size_t
#ifdef ENABLE_LOGGING
  #include <cstdio>  // std::printf
  #include <chrono>
#endif
#include "LinkedListConcept.hpp"

enum ModificationType {
  UNKNOWN,
  ADD,
  REMOVE
};

std::string toString(const ModificationType& type) {
  switch (type) {
    case ModificationType::ADD:
      return "ADD";
    case ModificationType::REMOVE:
      return "REM";
    case ModificationType::UNKNOWN:
    default:
      assert(false);
      return "";
  }
}

template <typename T>
class LazyList {
public:
  LazyList() {
    head = std::make_shared<MarkedNode<T>>(T(), 0);
    tail = std::make_shared<MarkedNode<T>>(T(), std::numeric_limits<std::size_t>::max());
    size = 0;
    head->next = tail;

    #ifdef ENABLE_LOGGING
      start = std::chrono::high_resolution_clock::now();
    #endif
  }
  bool add(const T& val) {
    return modifyList(ModificationType::ADD, val);
  }

  bool remove(const T& val) {
    return modifyList(ModificationType::REMOVE, val);
  }

  bool contains(const T& val) {
    std::size_t key = std::hash<T>{}(val);
    std::shared_ptr<MarkedNode<T>> pred = head;
    std::shared_ptr<MarkedNode<T>> curr = head->next;
    while (key > curr->key) {
      pred = curr;
      curr = curr.next;
    }
    return !curr->removed && curr->next == key;
  }

private:
  /* Check that pred and curr have not been removed, and that pred points to curr */
  bool validate (std::shared_ptr<MarkedNode<T>>& pred, std::shared_ptr<MarkedNode<T>>& curr) {
    return !pred->removed && !curr->removed && pred->next == curr;
  }

  /* Add, remove have very repetitive code, so group together */
  bool modifyList(const ModificationType& type, const T& val) {
    std::size_t key = std::hash<T>{}(val);
    std::cout << "val " << val << std::endl;
    std::cout << "key " << key << std::endl;
    std::cout << "tail->key " << tail->key << std::endl;
    std::cout << "max " << std::numeric_limits<std::size_t>::max() << std::endl;
    while (true) {
      std::shared_ptr<MarkedNode<T>> pred = head;
      std::shared_ptr<MarkedNode<T>> curr = head->next;
      int i = 0;
      while (curr->key < key) {
        if (i++ >= 3) {
          break;
        }
        std::cout << "wtf " << i << "," << curr->key << std::endl;
        pred = curr;
        curr = curr->next;
      }

      // local list structure of ...->[PRED]->[CURR]->... may be modified by another thread in this interval...

      std::lock_guard<std::mutex> predLock{pred->mutex};
      std::lock_guard<std::mutex> currLock{curr->mutex};
      if (validate(pred, curr)) {
        bool isSuccessful = false;
        switch (type) {
          case ModificationType::ADD:
            if (curr->key != key) {
              std::cout << "diagnose" << std::endl;
              std::shared_ptr<MarkedNode<T>> node = std::make_shared<MarkedNode<T>>(val);
              node->next = curr;
              pred->next = node;
              size++;
              isSuccessful = true;
            }
            break;
          case ModificationType::REMOVE:
            std::cout << "suspicious " << std::endl;
            if (curr->key == key) {
              curr->removed = true;
              pred->next = curr->next;
              size--;
              isSuccessful = true;
            }
            break;
          case ModificationType::UNKNOWN:
          default:
            assert(false);
            break;
        }
        std::cout << "expected " << std::endl;

        #ifdef ENABLE_LOGGING
          std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
          std::chrono::microseconds time = std::chrono::duration_cast<std::chrono::microseconds>(now-start);
          std::printf("%10x,%5s,%5d,%5s,%5d,%5d\n",
                      std::this_thread::get_id(),
                      toString(type).c_str(),
                      key,
                      isSuccessful ? "true" : "false",
                      size,
                      time);
          printList();
        #endif
        return isSuccessful;
      }
    }
  }

  void printList() {
    std::shared_ptr<MarkedNode<T>> curr = head;
    std::cout << "{";
    while (curr->key < tail->key) {
      std::cout << curr->key << ", ";
      curr = curr->next;
    }
    std::cout << "}" << std::endl;
  }

  std::shared_ptr<MarkedNode<T>> head;
  std::shared_ptr<MarkedNode<T>> tail;
  std::size_t size;
  #ifdef ENABLE_LOGGING
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
  #endif
};

#endif

