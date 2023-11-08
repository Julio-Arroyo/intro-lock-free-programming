/* Simple program experimenting with usage of lock guard */

#include <thread>
#include <chrono>
#include <iostream>
#include <mutex>
#include <cassert>

class Counter {
public:
  void increment() {
    counter++;
  }

  std::size_t get() {
    return counter;
  }

private:
  std::size_t counter{0};
};

int main(int argc, char** argv) {
    if (argc != 1) {
        return -1;
    }

    Counter counter{};
    Counter counter2{};
    std::mutex mutex;
    int num_threads = 8;
    int work_per_thread = 1000000 / num_threads;

    auto f = [&mutex, &counter, &counter2, &work_per_thread]() {
        for (int i = 0; i < work_per_thread; i++) {
            counter2.increment();  // not protected by lock guard

            std::lock_guard<std::mutex> counterLock(mutex);
            counter.increment();  // protected by lock guard
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

    std::cout << "Counter1: " << counter.get() << std::endl;  // 1M, since counter was below lock guard in loop
    std::cout << "Counter2: " << counter2.get() << std::endl;  // not 1M, since counter was above lock guard

    return 0;
}
