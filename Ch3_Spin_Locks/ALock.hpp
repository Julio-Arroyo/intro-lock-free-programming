#ifndef ALOCK_HPP
#define ALOCK_HPP

#include <atomic>

#define MAX_NO_THREADS 8

class ALock {
public:
    ALock() {
        ready[0] = true;
    }

    /* Return's the slot acquired by the caller. Caller must
     * pass the returned 'slot' value to the corresponding unlock call. */
    int lock() {
        int slot = (tail++) % MAX_NO_THREADS;
        while (!ready[slot]) {}
        return slot;
    }

    void unlock(const int slot) {
        ready[slot] = false;
        ready[(slot+1) % MAX_NO_THREADS] = true;
    }

private:
    bool ready[MAX_NO_THREADS];
    std::atomic_int tail{0};
};

#endif

