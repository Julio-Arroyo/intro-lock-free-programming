#ifndef LOCK_FREE_LIST_HPP
#define LOCK_FREE_LIST_HPP

#include <memory>   // shared_ptr
#include <tuple>    // tie
#include <utility>  // pair
#ifdef ENABLE_LOGGING
  #include <chrono>
#endif
#include "LinkedListConcept.hpp"
#include "AtomicMarkableReference.hpp"

template <typename T>
class LockFreeList {
public:
  LockFreeList() {
    head = std::make_shared<LockFreeNode<T>>(T(), 0);
    tail = std::make_shared<LockFreeNode<T>>(T(), std::numeric_limits<std::size_t>::max());
    head->setNext(tail);
    
    #ifdef ENABLE_LOGGING
      start = std::chrono::high_resolution_clock::now();
    #endif
  }

  bool add(const T& val) {
    std::size_t key = std::hash<T>{}(val);
    bool isSuccessful{false};
    while (true) {
      std::shared_ptr<T> pred, curr;
      std::tie(pred, curr) = find(key);

      if (curr->key == key) {
        isSuccessful = false;
        break;
      } else {
        // create newNode
        std::shared_ptr<LockFreeNode<T>> newNode = std::make_shared<LockFreeNode<T>>(val);
        // point newNode->curr
        newNode->setNext(curr);

        // try to point pred -> newNode
        if (pred->next.compareAndSet(curr, newNode, false, false)) {
          size++;
          isSuccessful = true;
          break;
        }
        
        // could not point pred->newNode... restarting add()

        // pred->newNode fails if another thread either
        //   - marks pred as removed
        //   - inserts some other node after pred
        // but note that it not fails if curr is removed,
        // showing why it is needed to physically remove nodes in find()
      }
    }

    #ifdef ENABLE_LOGGING
      std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
      std::chrono::microseconds time = std::chrono::duration_cast<std::chrono::microseconds>(now-start);
      std::printf("%10x,%5s,%5d,%5s,%5d,%5d\n",
                  std::this_thread::get_id(),
                  "ADD",
                  key,
                  isSuccessful ? "true" : "false",
                  size,
                  time);
    #endif

    return isSuccessful;
  }

  bool remove(const T& val) {
    std::size_t key = std::hash<T>{}(val);
    bool isSuccessful{false};
    while (true) {
      std::shared_ptr<T> pred, curr;
      std::tie(pred, curr) = find(key);

      if (curr->key != key) {
        isSuccessful = false;
        break;
      } else {
        // attempt to logically remove, restart if failed
        if (curr->attemptMarkAsRemoved()) {
          // CAS pred -> curr.next ?
          isSuccessful = true;
          size--;
          break;
        }
      }
    }

    #ifdef ENABLE_LOGGING
      std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
      std::chrono::microseconds time = std::chrono::duration_cast<std::chrono::microseconds>(now-start);
      std::printf("%10x,%5s,%5d,%5s,%5d,%5d\n",
                  std::this_thread::get_id(),
                  "REM",
                  key,
                  isSuccessful ? "true" : "false",
                  size,
                  time);
    #endif

    return isSuccessful;
  }

  bool contains(const T& val) {
    std::size_t key = std::hash<T>{}(val);
    bool isCurrRemoved[1] = {false};
    std::shared_ptr<LockFreeNode<T>> curr = head.getReference();
    while (curr->key < key) {
      curr = curr->next.getReference();
    }
    return curr->key == key && !curr->isRemoved();
  }

private:
  std::pair<std::shared_ptr<LockFreeNode<T>>,
            std::shared_ptr<LockFreeNode<T>>>
  find(const std::size_t key) {
    while (true) {
      std::shared_ptr<LockFreeNode<T>> pred = head.getReference();
      std::shared_ptr<LockFreeNode<T>> curr = pred->next;
      while (curr->key < key) {
        // if curr is logically removed, try to physically remove it
        if (curr->isRemoved()) {
          if (!pred->next.compareAndSet(curr, curr->next.getReference(),
                                        false, false)) {
            // restart find() if could not physically remove
            //   - pred might have been removed
            //   - a node might have been inserted between pred and curr
            break;
          }
        }
        pred = curr;
        curr = curr->next.getReference();
      }

      return std::make_pair(pred, curr);
    }

    // unreachable
    assert(false);
    return std::make_pair(nullptr, nullptr);
  }

  AtomicMarkableReference<LockFreeNode<T>> head{nullptr, false};
  AtomicMarkableReference<LockFreeNode<T>> tail{nullptr, false};
  std::size_t size{0};
  #ifdef ENABLE_LOGGING
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
  #endif
};

#endif

