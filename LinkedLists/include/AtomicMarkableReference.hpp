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
    std::shared_ptr<T> oldMarkedRef{(T*) ((uintptr_t) expectedRef | expectedMark)};
    std::shared_ptr<T> newMarkedRef{(T*) ((uintptr_t) newRef | newMark)};
    return markedRef.compare_exchange_strong(oldMarkedRef, newMarkedRef);
  }

  bool attemptMark(const T* expectedRef, bool newMark) {
    // TODO: what if it is already marked?
    std::shared_ptr<T> newMarkedRef{(T*) ((uintptr_t) expectedRef | newMark)};
    std::shared_ptr<T> oldUnmarkedRef{expectedRef};
    return markedRef.compare_exchange_strong(oldUnmarkedRef, newMarkedRef);
  }

  std::shared_ptr<T> getReference() {
    return std::shared_ptr<T>(((uintptr_t) markedRef.load()) & ((uintptr_t) ~1));
  }

  std::pair<std::shared_ptr<T>, bool> getRefAndMark() {
    uintptr_t marked_ref = (uintptr_t) markedRef.load();
    
    std::shared_ptr<T> ref{(marked_ref & ((uintptr_t) ~1))};
    bool mark = marked_ref & 1;

    return std::make_pair<std::shared_ptr<T>, bool>(ref, mark);
  }

  // Atomically check whether the reference has been marked
  bool isMarked() {
    return ((uintptr_t) markedRef.load()) & 1;
  }

private:
  std::atomic<std::shared_ptr<T>> markedRef{nullptr};

  // TODO: this class should be no more than a wrapper, only field should be the ptr.
  // I believe it can be done with a atomic_uintptr_t, so try to do it with atomic<shared_ptr> to get benefits of
  // automatic memory deallocation.
};

#endif

