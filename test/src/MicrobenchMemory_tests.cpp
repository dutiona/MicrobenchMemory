#include "MicrobenchMemory/MicrobenchMemory.hpp"
#include "MicrobenchMemory/ext/io.hpp"

#include <iostream>

#include <gtest/gtest.h>


MICROBENCH_MEMORY_OVERLOAD_GLOBAL()


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