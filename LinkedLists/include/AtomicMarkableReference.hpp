#ifndef ATOMIC_MARKABLE_REFERENCE_HPP
#define ATOMIC_MARKABLE_REFERENCE_HPP

#include <memory>  // shared_ptr
#include <atomic>  // atomic
#include <utility>  // pair
#include <iostream>

template <typename T>
class AtomicMarkableReference {
public:
  AtomicMarkableReference(T* ref, bool mark) {
    int mark_mask = mark ? 1 : 0;
    T* marked_ref = (T*) ((uintptr_t) ref | mark_mask);
    markedRef.store(std::shared_ptr<T>{marked_ref});
  }

  bool compareAndSet(const std::shared_ptr<T> expectedRef,
                     const std::shared_ptr<T> newRef,
                     bool expectedMark,
                     bool newMark) {
    assert(false);
    return false;
  }

  bool attemptMark(const T& expectedRef, bool newMark) {
    assert(false);
    return false;
  }

  std::shared_ptr<T> getReference() {
    assert(false);
    return nullptr;
  }

  std::pair<std::shared_ptr<T>, bool> getRefAndMark() {
    return nullptr;
  }

  bool isMarked() {
    return false;
  }

private:
  std::atomic<std::shared_ptr<T>> markedRef{nullptr};
  // TODO: this class should be no more than a wrapper, only field should be the ptr.
  // I believe it can be done with a atomic_uintptr_t, so try to do it with atomic<shared_ptr> to get benefits of
  // automatic memory deallocation.
};

#endif

