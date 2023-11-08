#ifndef OPTIMISTIC_LIST_HPP
#define OPTIMISTIC_LIST_HPP

#include <mutex>
#include "LinkedListConcept.hpp"

template <typename T>
class OptimisticList {
public:
  OptimisticList() {
    tail = Node{std::numeric_limits<std::size_t>::max(), std::mutex(), T(), Node{}};
    head = Node{0, std::mutex(), T(), tail};
    size = 0;
  }

  /* Returns true iff val was not present before adding it */
  bool add(const T& val) {
    Node pred = head;
    Node curr = head.next;
    std::size_t newKey = std::hash<T>{}(val);

    while (true) {  // loop repeatedly until validate() succeeds
      while (curr.key < newKey) {  // loop until pred.key < newKey <= curr.key
        pred = curr;
        curr = curr.next;
      }

      std::lock_guard<std::mutex> predLock(pred.lock);
      std::lock_guard<std::mutex> currLock(curr.lock);
      if (validate(pred, curr)) {
        if (curr.key != newKey) {  // add newNode if not present
          Node newNode = Node{newKey, std::mutex(), val, curr};
          pred.next = newNode;
          size++;
          return true;
        }
        return false;
      }
    }
    assert(false, "ERROR: unreachable");
  } 

  /* Returns true if val was removed from the set, false if it was not present */
  bool remove(const T& val) {
    Node pred = head;
    Node curr = head.next;
    std::size_t removedKey = std::hash<T>{}(val);

    while (true) {
      while (curr.key < removedKey) {
        pred = curr;
        cur = curr.next;
      }

      std::lock_guard<std::mutex> predLock(pred.lock);
      std::lock_guard<std::mutex> currLock(curr.lock);
      if (validate(pred, curr)) {
        if (curr.key == removedKey) {
          pred.next = curr.next;
          size--;
          return true;
        } else {
          return false;
        }
      }
    }
  }

  bool contains(const T& val) {
    Node pred = head;
    Node curr = head.next;
    std::size_t searchKey = std::hash<T>{}(val);

    while (true) {
      while (curr.key < searchKey) {
        pred = curr;
        cur = curr.next;
      }

      std::lock_guard<std::mutex> predLock(pred.lock);
      std::lock_guard<std::mutex> currLock(curr.lock);
      if (validate(pred, curr)) {
        return curr.key == searchKey;
      }
    }
  }

private:
  bool validate(const Node& pred, const Node& curr) {
    Node node = head;
    while (true) {
      if (node.key > pred.key) {
        return false;  // pred not found
      } else if (node.key == pred.key) {
        return pred.next.key = curr.key;
      }
      node = node.next;
    }
  }

  struct Node {
    std::size_t key;
    std::mutex mutex;
    T val;
    Node next;
  }

  Node head;
  Node tail;
  std::size_t size;
};
static_assert(LinkedListConcept<OptimisticList<int>>);  // TODO: can 'int' be made arbitrary?

#endif

