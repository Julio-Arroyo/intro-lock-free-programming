#include <iostream>
#include <new>
#include <array>
#include <thread>

struct alignas(std::hardware_destructive_interference_size)
alignedQueueElem {
    int x;
};

int main() {
    static_assert(__cpp_lib_hardware_interference_size);

    size_t cacheLineSize = std::hardware_destructive_interference_size;
    std::cout << "Cache line size: " << cacheLineSize << std::endl;
    
    std::array<int64_t, 4> arr{69, 70, 71, 72};
    std::array<alignedQueueElem, 4> arrAl{42, 43, 44, 45};

    std::cout << "arr size " << arr.size() << std::endl;
    std::cout << "arrAl size " << arrAl.size() << std::endl;

    // Note the differing addresses for the elements of each array.
    for (int i = 0; i < 4; i++) {
        std::cout << "arr[" << i << "] address: " << &arr[i] << std::endl;
    }

    for (int i = 0; i < 4; i++) {
        std::cout << "arrAl[" << i << "] address: " << &arrAl[i] << std::endl;
    }

    return 0;
}

