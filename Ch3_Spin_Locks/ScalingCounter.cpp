#include "TASlock.hpp"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    int counter = 0;
    TTASlock mutex;

    auto f = [&mutex, &counter]() {
        bool firstTime = true;
        std::chrono::time_point<std::chrono::high_resolution_clock> start;
        for (int i = 0; i < 1000000; i++) {
            mutex.lock();
            if (firstTime) {
                firstTime = false;
            } else {
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                std::cout << std::this_thread::get_id() << "," << i << "," << duration.count() << std::endl;
            }
            counter++;
            mutex.unlock();
            start = std::chrono::high_resolution_clock::now();
        }
    };

    std::thread thread1(f);
    // std::thread thread2(f);
    // std::thread thread3(f);
    // std::thread thread4(f);
    // std::thread thread5(f);
    // std::thread thread6(f);
    // std::thread thread7(f);
    // std::thread thread8(f);

    thread1.join();
    // thread2.join();
    // thread3.join();
    // thread4.join();
    // thread5.join();
    // thread6.join();
    // thread7.join();
    // thread8.join();

    return 0;
}
