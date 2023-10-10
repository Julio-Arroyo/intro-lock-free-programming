#include <thread>
#include <iostream>
#include "Concepts.hpp"

class PetersonNaive {
public:
    void lock() {
        std::thread::id currThreadID = std::this_thread::get_id();

        // mark first thread to acquire lock
        std::thread::id defaultID = std::thread::id();
        if (firstThread.compare_exchange_strong(defaultID,
                                                currThreadID,
                                                std::memory_order_seq_cst,
                                                std::memory_order_seq_cst)) {
            std::cout << "FIRST THREAD: " << firstThread << std::endl;
        }

        int thisID = currThreadID == firstThread.load();
        int otherID = 1 - thisID;
        interested[thisID] = true;
        waiter = thisID;
        while (interested[otherID] && waiter == thisID) {}
    }

    void unlock() {
        int thisID = std::this_thread::get_id() == firstThread.load();
        interested[thisID] = false;
    }

private:
    bool interested[2] = {false, false};
    int waiter{-1};
    std::atomic<std::thread::id> firstThread;
};
static_assert(LockConcept<PetersonNaive>);

class PetersonGood {
public:
    void lock() {
        std::thread::id currThreadID = std::this_thread::get_id();

        // mark first thread to acquire lock
        std::thread::id defaultID = std::thread::id();
        if (firstThread.compare_exchange_strong(defaultID,
                                                currThreadID,
                                                std::memory_order_seq_cst,
                                                std::memory_order_seq_cst)) {
            std::cout << "FIRST THREAD: " << firstThread << std::endl;
        }

        int thisID = currThreadID == firstThread.load();
        int otherID = 1 - thisID;
        interested[thisID].store(true);
        waiter.store(thisID);
        while (interested[otherID].load() && waiter.load() == thisID) {}
    }

    void unlock() {
        int thisID = std::this_thread::get_id() == firstThread.load();
        interested[thisID].store(false);
    }

private:
    std::atomic_bool interested[2] = {false, false};
    std::atomic_int8_t waiter{-1};
    std::atomic<std::thread::id> firstThread;
};
static_assert(LockConcept<PetersonGood>);
