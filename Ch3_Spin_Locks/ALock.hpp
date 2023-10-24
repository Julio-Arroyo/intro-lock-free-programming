#ifndef ALOCK_HPP
#define ALOCK_HPP

#define MAX_NO_THREADS 8

class ALock {
public:
    ALock() {
        ready[0] = true;
    }

    void lock() {
        slot = (tail++) % MAX_NO_THREADS;
        while (!ready[slot]) {}
    }

    void unlock() {
        ready[slot] = false;
        ready[(slot+1) % MAX_NO_THREADS] = true;
    }

private:
    bool ready[MAX_NO_THREADS];
    atomic_int tail{0};
    thread_local int slot;
}

#endif

