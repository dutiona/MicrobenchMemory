#include "MicrobenchMemory/MicrobenchMemory.hpp"
#include "MicrobenchMemory/ext/io.hpp"

#include <iostream>

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>


struct A
{
  char buff[256];
  MICROBENCH_MEMORY_INSTRUMENT_CLASS("A");
};

TEST_CASE("1: All test cases reside in other .cpp files (empty)", "[multi-file:1]")
{

  [[maybe_unused]] A a{};

  auto mem_infos_1 = MicrobenchMemory::get_memory_information_snapshot("A");
  std::cout << mem_infos_1;

  auto* ptr = new A{};

  auto mem_infos_2 = MicrobenchMemory::get_memory_information_snapshot("A");
  std::cout << mem_infos_2;

  delete ptr;

  auto mem_infos_3 = MicrobenchMemory::get_memory_information_snapshot("A");
  std::cout << mem_infos_3;
}


int main(int argc, char* argv[])
{
  int result = Catch::Session().run(argc, argv);
  return result;
}


// MICROBENCH_MEMORY_OVERLOAD_GLOBAL()

/*

TEST(MicrobenchMemory, OneAllocation)
{
  auto mem_infos_clean = MicrobenchMemory::get_global_memory_information_snapshot();
  std::cout << mem_infos_clean;

  int* a = new int;
  *a     = 1;

  auto mem_infos_after_alloc = MicrobenchMemory::get_global_memory_information_snapshot();
  std::cout << mem_infos_after_alloc;

  delete a;

  auto mem_infos_after_dealloc = MicrobenchMemory::get_global_memory_information_snapshot();
  std::cout << mem_infos_after_dealloc;
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

*/
/*

int main()
{
  auto mem_infos_clean = MicrobenchMemory::get_global_memory_information_snapshot();
  std::cout << mem_infos_clean;

  int* a = new int;
  *a     = 1;

  auto mem_infos_after_alloc = MicrobenchMemory::get_global_memory_information_snapshot();
  std::cout << mem_infos_after_alloc;

  delete a;

  auto mem_infos_after_dealloc = MicrobenchMemory::get_global_memory_information_snapshot();
  std::cout << mem_infos_after_dealloc;

  double* b = new double[2048];

  auto mem_infos_2 = MicrobenchMemory::get_global_memory_information_snapshot();
  std::cout << mem_infos_2;

  delete[] b;

  auto mem_infos_3 = MicrobenchMemory::get_global_memory_information_snapshot();
  std::cout << mem_infos_3;
}
*/