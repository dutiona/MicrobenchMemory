#include "MicrobenchMemory/MicrobenchMemory.hpp"
#include "MicrobenchMemory/ext/io.hpp"

#include <iostream>

#include <gtest/gtest.h>

MICROBENCH_MEMORY_OVERLOAD_GLOBAL()


TEST(MicrobenchMemory, GlobalOneAllocation)
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


struct A
{
  char buff[256];
  MICROBENCH_MEMORY_INSTRUMENT_CLASS("A");
};

TEST(MicrobenchMemory, ScopedOneAllocation)
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

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
