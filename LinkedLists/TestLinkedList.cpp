#include <cstdlib>
#include <thread>
#include <string>
#include <cstring>
#include "CoarseList.hpp"
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

void testSPSC() {
  std::srand(2021);  // set random seed
  CoarseList<int> lst{};
  #ifdef ENABLE_LOGGING
  std::cout << "THREAD_ID,OPE,VAL,RET,SIZ,TS" << std::endl;
  #endif
  int work = 42;

  auto produce = [&lst, &work]() {
    for (int i = 0; i < 1; i++) {
      if (i == 5)
        lst.contains(work);
      lst.add(work);
    }
  };

  auto consume = [&lst, &work]() {
    for (int i = 0; i < 1; i++) {
      if (i == 5)
        lst.contains(work);
      lst.remove(work);
    }
  };

  std::thread thread1(produce);
  std::thread thread2(consume);

  thread1.join();
  thread2.join();
}

int main(int argc, char** argv) {
  if (argc != 3) {
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
    std::cerr << "[LIST_TYPE] argument shoulde be one of:" << std::endl;
    std::cerr << "\t - 'C' for coarselist" << std::endl;
  }

  char mode = argv[1][0];
  char list_type = argv[2][0];

  if (list_type == 'C') {
    if (mode == 'S') {
      CoarseList<std::string> lst{};
      singleThreadedTest<std::string>(lst);
    } else if (mode == 'M') {
      testSPSC();
    } else {
      std::cerr << "Unknown mode: '" << mode << "'" << std::endl;
    }
    return 0;
  } else {
    std::cerr << "Unkwown list type '" << list_type << "'" << std::endl;
    return -1;
  }
  return 0;
}

