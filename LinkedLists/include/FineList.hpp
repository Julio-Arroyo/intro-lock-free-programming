#ifndef FINE_LIST_HPP
#define FINE_LIST_HPP

#include "LinkedListConcept.hpp"

template<typename T>
class FineList {
public:
  FineList() : size{0} {
    head = std::make_shared<Node<T>>(T(), 0);
    tail = std::make_shared<Node<T>>(T(), std::numeric_limits<std::size_t>::max());
    head->next = tail;

    #ifdef ENABLE_LOGGING
    std::cout << "LOG: FineList class" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    #endif
  }

  bool add(const T& val) {
    std::size_t key = std::hash<T>{}(val);
    std::shared_ptr<Node<T>> pred = head;
    pred->mutex.lock();
    std::shared_ptr<Node<T>> curr = head->next;
    curr->mutex.lock();

    while (curr->key < key) {
      pred->mutex.unlock();
      pred = curr;
      curr = curr->next;
      curr->mutex.lock();
    }

    bool isSuccessful = false;
    if (curr->key != key) {
      std::shared_ptr<Node<T>> node = std::make_shared<Node<T>>(val);
      node->next = curr;
      pred->next = node;
      isSuccessful = true;
      size++;
    }

    #ifdef ENABLE_LOGGING
      std::chrono::time_point<std::chrono::high_resolution_clock>
        now = std::chrono::high_resolution_clock::now();
      std::chrono::microseconds
        time = std::chrono::duration_cast<std::chrono::microseconds>(now-start);
      std::printf("%x,ADD,%d,%s,%d,%d\n",
                  std::this_thread::get_id(),
                  key,
                  isSuccessful ? "true" : "false",
                  size,
                  time);
    #endif

    pred->mutex.unlock();
    curr->mutex.unlock();

    return isSuccessful;
  }

  bool remove(const T& val) {
    std::size_t key = std::hash<T>{}(val);
    std::shared_ptr<Node<T>> pred = head;
    pred->mutex.lock();
    std::shared_ptr<Node<T>> curr = head->next;
    curr->mutex.lock();

    while (curr->key < key) {
      pred->mutex.unlock();
      pred = curr;
      curr = curr->next;
      curr->mutex.lock();
    }

    bool isSuccessful = false;
    if (curr->key == key) {
      pred->next = curr->next;
      isSuccessful = true;
      size--;
    }

    #ifdef ENABLE_LOGGING
      std::chrono::time_point<std::chrono::high_resolution_clock>
        now = std::chrono::high_resolution_clock::now();
      std::chrono::microseconds
        time = std::chrono::duration_cast<std::chrono::microseconds>(now-start);
      std::printf("%x,REM,%d,%s,%d,%d\n",
                  std::this_thread::get_id(),
                  key,
                  isSuccessful ? "true" : "false",
                  size,
                  time);
    #endif

    pred->mutex.unlock();
    curr->mutex.unlock();

    return isSuccessful;
  }

  bool contains(const T& val) {
    std::size_t key = std::hash<T>{}(val);
    std::shared_ptr<Node<T>> pred = head;
    pred->mutex.lock();  // lock head b4 getting next, avoid RC1 (see README)
    std::shared_ptr<Node<T>> curr = head->next;
    curr->mutex.lock();

    while (curr->key < key) {
      pred->mutex.unlock();
      pred = curr;
      curr = curr->next;
      curr->mutex.lock();
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
      std::printf("%x,CON,%d,%s,%d,%d\n",
                  std::this_thread::get_id(),
                  key,
                  isSuccessful ? "true" : "false",
                  size,
                  time);
    #endif

    pred->mutex.unlock();
    curr->mutex.unlock();

    return isSuccessful;
  }

private:
  std::shared_ptr<Node<T>> head, tail;
  std::size_t size;
  #ifdef ENABLE_LOGGING
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  #endif
};

#endif

