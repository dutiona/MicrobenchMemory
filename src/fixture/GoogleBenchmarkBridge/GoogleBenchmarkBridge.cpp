#include "fixture/GoogleBenchmarkBridge/GoogleBenchmarkBridge.hpp"
#include "MicrobenchMemory/MicrobenchMemory.hpp"

#include <benchmark/benchmark.h>

#include <cstddef>

namespace MicrobenchMemory::fixtures::GoogleBenchmarkBridge
{

  void MemoryManager::Start() { start_ = MicrobenchMemory::get_global_memory_information_snapshot(); }

  void MemoryManager::Stop(::benchmark::MemoryManager::Result* result)
  {
    auto stop              = MicrobenchMemory::get_global_memory_information_snapshot();
    auto diff              = stop - start_;
    result->num_allocs     = static_cast<std::ptrdiff_t>(diff.allocation_count);
    result->max_bytes_used = static_cast<std::ptrdiff_t>(diff.total_memory_allocated);
  }

} // namespace MicrobenchMemory::fixtures::GoogleBenchmarkBridge
