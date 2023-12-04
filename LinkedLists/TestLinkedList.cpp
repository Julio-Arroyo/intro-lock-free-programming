#include <cstdlib>
#include <thread>
#include <string>
#include <cstring>

#include "CoarseList.hpp"
#include "FineList.hpp"
#include "OptimisticList.hpp"
#include "LazyList.hpp"

template<typename E, LinkedListConcept<E> LinkedList>
void singleThreadedTest(LinkedList& lst) {
  std::printf("%10s,%5s,%5s,%5s,%5s,%5s\n",
              "THREAD_ID",
              "OPE",
              "VAL",
              "RET",
              "SIZE",
              "TS");
  assert(lst.add("2"));
  assert(lst.add("1"));
  assert(lst.add("0"));
  assert(!lst.remove("8"));
  assert(!lst.remove("3"));
  assert(!lst.remove("9"));
  assert(!lst.add("1"));
  assert(lst.remove("1"));
  assert(lst.add("1"));
  assert(lst.add("3"));
  assert(!lst.add("2"));
  std::cout << "Single threaded tests pass." << std::endl << std::endl;
}

template<typename E, LinkedListConcept<E> LinkedList>
void singleThreadedTest2(LinkedList& lst) {
  std::printf("%10s,%5s,%5s,%5s,%5s,%5s\n",
              "THREAD_ID",
              "OPE",
              "VAL",
              "RET",
              "SIZE",
              "TS");
  assert(lst.add(2));
  assert(lst.add(1));
  assert(lst.add(4));
  assert(!lst.remove(8));
  assert(!lst.remove(3));
  assert(!lst.remove(9));
  assert(!lst.add(1));
  assert(lst.remove(1));
  assert(lst.add(1));
  assert(lst.add(3));
  assert(!lst.add(2));
  std::cout << "Single threaded tests pass." << std::endl << std::endl;
}

template <typename E, LinkedListConcept<E> LinkedList>
void testSPSC(LinkedList& lst) {
  #ifndef ENABLE_LOGGING
    std::cerr << "Must define ENABLE_LOGGING macro for multi-threaded tests." << std::endl;
    assert(false);
  #endif

  std::cout << "*** Begin Test SPSC ***" << std::endl;
  std::srand(2021);  // set random seed
  std::printf("%10s,%5s,%5s,%5s,%5s,%5s\n",
              "THREAD_ID",
              "OPE",
              "VAL",
              "RET",
              "SIZE",
              "TS");
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
  std::printf("%10s,%5s,%5s,%5s,%5s,%5s\n",
              "THREAD_ID",
              "OPE",
              "VAL",
              "RET",
              "SIZE",
              "TS");
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

std::string getModeString() {
  return std::string("[MODE] argument should be one of:\n") +
         std::string("\t - 'S' for single-threaded testing\n") +
         std::string("\t - 'M' for multi-threaded testing\n"); 
}

std::string getListTypeString() {
  return std::string("[LIST_TYPE] argument should be one of:\n") +
         std::string("\t - 'C' for coarselist\n") + 
         std::string("\t - 'F' for FineList\n") +
         std::string("\t - 'O' for OptimisticList\n") +
         std::string("\t - 'L' for LazyList\n");
}

std::string getUsageString() {
  return std::string("USAGE:\n") +
         std::string("\t'./test [MODE] [LIST_TYPE]'\n") +
         getModeString() +
         getListTypeString();
}

int main(int argc, char** argv) {
  if (argc == 1) {
    std::cout << getUsageString();
    return 0;
  } else if (argc != 3) {
    std::cerr << getUsageString();
    return -1;
  }

  if (strlen(argv[1]) != 1) {
    std::cerr << getModeString();
    return -1;
  }
  if (strlen(argv[2]) != 1) {
    std::cerr << getListTypeString();
    return -1;
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
  } else if (list_type == 'O') {
    if (mode == 'S') {
      OptimisticList<std::string> lst{};
      singleThreadedTest<std::string>(lst);
      return 0;
    } else if (mode == 'M') {
      OptimisticList<int> lst{};
      testSPSC<int>(lst);
      testRandomSPSC<int>(lst);
      return 0;
    } else {
      std::cerr << "Unknown mode: '" << mode << "'" << std::endl;
      return -1;
    }
  } else if (list_type == 'L') {
    if (mode == 'S') {
      LazyList<int> lst{};
      singleThreadedTest2<int>(lst);
      // LazyList<std::string> lst{};
      // singleThreadedTest<std::string>(lst);
      return 0;
    } else if (mode == 'M') {
      LazyList<int> lst{};
      testSPSC<int>(lst);
      testRandomSPSC<int>(lst);
      return 0;
    }
  }

  std::cerr << "Unknown list type '" << list_type << "'" << std::endl;
  return -1;
}

