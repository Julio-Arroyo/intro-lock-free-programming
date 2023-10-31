#ifndef CACHE_AWARE_ALOCK_HPP
#define CACHE_AWARE_ALOCK_HPP

#include <new>  // std::hardware_destructive_interference_size
#include <array>
#include <atomic>
#include <iostream>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winterference-size"

#define CACHE_LINE_SIZE std::hardware_destructive_interference_size
#define MAX_NO_THREADS 8

class CacheAwareALock {
public:
    CacheAwareALock() {
       ready[0] = {true};
    }

    int lock() {
        int slot = (tail++) % MAX_NO_THREADS;
        while (!ready[slot].flag) {}
        return slot;
    }

    void unlock(const int slot) {
        ready[slot] = {false};
        ready[(slot+1) % MAX_NO_THREADS] = {true};
    }

private:
    struct alignas(CACHE_LINE_SIZE) ArrElem {
        bool flag;
    };

    std::array<ArrElem, MAX_NO_THREADS> ready;
    alignas(CACHE_LINE_SIZE) std::atomic_int tail{0}; 
};

#pragma GCC diagnostic pop

#endif

