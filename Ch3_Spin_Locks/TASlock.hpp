#include <atomic>

class TASlock
{
public:
    void lock() {
        while (flag.exchange(true)) {}
    }

    void unlock() {
        flag.store(false);
    }

private:
    std::atomic_bool flag{false};
};

class TTASlock
{
public:
    void lock() {
        while (true) {
            while (flag.load()) { }
            if (!flag.exchange(true)) {
                return;
            }
        }
    }

    void unlock() {
        flag.store(false);
    }

private:
    std::atomic_bool flag{false};
};
