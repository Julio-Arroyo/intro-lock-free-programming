#include "TASlock.hpp"
#include <thread>
#include <chrono>

int main() {
    int counter = 0;
    TTASlock mutex;

    auto f = [&mutex, &counter]() {
        for (int i = 0; i < 1000000; i++) {
            mutex.lock();
            counter++;
            mutex.unlock();
        }
    };

    auto start = std::chrono::high_resolution_clock::now();

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

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);


    std::cout << "DURATION: " << duration.count() << ". ";
    std::cout <<  "RESULT: " << counter <<  std::endl;
    return 0;
}