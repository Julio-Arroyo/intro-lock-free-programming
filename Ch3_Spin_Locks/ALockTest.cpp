#include "ALock.hpp"
#include "CacheAwareALock.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <cassert>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Expected usage: "
                  << "'./scaling_counter NUM_THREADS'" << std::endl;
    }

    int counter = 0;
    ALock mutex;
    int num_threads = std::stoi(argv[1]);
    int work_per_thread = 1000000 / num_threads;

    auto f = [&mutex, &counter, &work_per_thread]() {
        for (int i = 0; i < work_per_thread; i++) {
            int slot = mutex.lock();
            counter++;
            mutex.unlock(slot);
        }
    };

    std::thread thread1(f);
    std::thread thread2(f);
    std::thread thread3(f);
    std::thread thread4(f);
    std::thread thread5(f);
    std::thread thread6(f);
    std::thread thread7(f);
    std::thread thread8(f);

    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();
    thread6.join();
    thread7.join();
    thread8.join();

    // if concurrent accesses to counter, it may not add up to 1M
    assert(counter == 1000000);

    return 0;
}
