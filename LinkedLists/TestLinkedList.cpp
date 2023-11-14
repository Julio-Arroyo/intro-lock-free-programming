#include <cstdlib>
#include <thread>
#include <string>
#include <cstring>
#include "CoarseList.hpp"
#include "FineList.hpp"
#include "OptimisticList.hpp"

template<typename E, LinkedListConcept<E> LinkedList>
void singleThreadedTest(LinkedList& lst) {
  assert(lst.add("2"));
  assert(lst.add("1"));
  assert(lst.add("0"));
  assert(!lst.remove("8"));
  assert(!lst.add("2"));
  assert(!lst.remove("9"));
  assert(!lst.add("1"));
  assert(lst.remove("1"));
  assert(lst.add("1"));
  assert(lst.add("3"));
  assert(!lst.add("2"));
  std::cout << "Single threaded tests pass." << std::endl << std::endl;
}

template <typename E, LinkedListConcept<E> LinkedList>
void testSPSC(LinkedList& lst) {
  #ifndef ENABLE_LOGGING
    std::cerr << "Define ENABLE_LOGGING macro for multi-threaded tests." << std::endl;
    assert(false);
  #endif

  std::cout << "*** Begin Test SPSC ***" << std::endl;
  std::srand(2021);  // set random seed
  std::cout << "THREAD_ID,OPE,VAL,RET,SIZ,TS" << std::endl;
  int work = 42;

  auto produce = [&lst, &work]() {
    for (int i = 0; i < 1; i++) {
      lst.add(work);
    }
  };

  auto consume = [&lst, &work]() {
    for (int i = 0; i < 1; i++) {
      lst.remove(work);
    }
  };

  std::thread thread1(produce);
  std::thread thread2(consume);

  thread1.join();
  thread2.join();
  std::cout << "*** End Test SPSC ***" << std::endl << std::endl;
}

template <typename E, LinkedListConcept<E> LinkedList>
void testRandomSPSC(LinkedList& lst) {
  #ifndef ENABLE_LOGGING
    std::cerr << "Define ENABLE_LOGGING macro for multi-threaded tests." << std::endl;
    assert(false);
  #endif

  std::cout << "*** Begin Test: Random SPSC ***" << std::endl;
  std::srand(2021);  // set random seed
  std::cout << "THREAD_ID,OPE,VAL,RET,SIZ,TS" << std::endl;
  int maxVal = 10;

  auto produce = [&lst, &maxVal]() {
    for (int i = 0; i < 10; i++) {
      double p = ((float) std::rand()) / RAND_MAX;
      int val = (int) maxVal * p;
      lst.add(val);
    }
  };

  auto consume = [&lst, &maxVal]() {
    for (int i = 0; i < 10; i++) {
      double p = ((float) std::rand()) / RAND_MAX;
      int val = (int) maxVal * p;
      lst.remove(val);
    }
  };

  std::thread thread1(produce);
  std::thread thread2(consume);

  thread1.join();
  thread2.join();

  std::cout << "*** End Test: Random SPSC ***" << std::endl << std::endl;
}

int main(int argc, char** argv) {
  if (argc == 1) {
    std::cout << "USAGE:" << std::endl;
    std::cout << "\t'./test [MODE] [LIST_TYPE]'" << std::endl;
    std::cout << "[MODE] argument should be one of:" << std::endl;
    std::cout << "\t - 'S' for single-threaded testing" << std::endl;
    std::cout << "\t - 'M' for multi-threaded testing" << std::endl;
    std::cout << "[LIST_TYPE] argument should be one of:" << std::endl;
    std::cout << "\t - 'C' for coarselist" << std::endl;
    std::cout << "\t - 'F' for FineList" << std::endl;
    return 0;
  } else if (argc != 3) {
    std::cerr << "Expected usage: ";
    std::cerr << "'./test [MODE] [LST_TYPE]'" << std::endl;
    return -1;
  }

  if (strlen(argv[1]) != 1) {
    std::cerr << "[MODE] argument should be one of:" << std::endl;
    std::cerr << "\t - 'S' for single-threaded testing" << std::endl;
    std::cerr << "\t - 'M' for multi-threaded testing" << std::endl;
    return -1;
  }
  if (strlen(argv[2]) != 1) {
    std::cerr << "[LIST_TYPE] argument should be one of:" << std::endl;
    std::cerr << "\t - 'C' for coarselist" << std::endl;
    std::cerr << "\t - 'F' for FineList" << std::endl;
  }

  char mode = argv[1][0];
  char list_type = argv[2][0];

  if (list_type == 'C') {
    if (mode == 'S') {
      CoarseList<std::string> lst{};
      singleThreadedTest<std::string>(lst);
    } else if (mode == 'M') {
      CoarseList<int> lst{};
      testRandomSPSC<int>(lst);
    } else {
      std::cerr << "Unknown mode: '" << mode << "'" << std::endl;
      return -1;
    }
    return 0;
  } else if (list_type == 'F') {
    if (mode == 'S') {
      FineList<std::string> lst{};
      singleThreadedTest<std::string>(lst);
    } else if (mode == 'M') {
      FineList<int> lst{};
      testRandomSPSC<int>(lst);
      testSPSC<int>(lst);
    } else {
      std::cerr << "Unknown mode: '" << mode << "'" << std::endl;
      return -1;
    }
    return 0;
  }

  std::cerr << "Unknown list type '" << list_type << "'" << std::endl;
  return -1;
}

