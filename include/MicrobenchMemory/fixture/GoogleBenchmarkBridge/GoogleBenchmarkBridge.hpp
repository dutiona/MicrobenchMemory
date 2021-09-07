#pragma once

#include "MicrobenchMemory/MicrobenchMemory.hpp"

#include <benchmark/benchmark.h>

namespace MicrobenchMemory::fixtures::GoogleBenchmarkBridge
{

  class MemoryManager : public ::benchmark::MemoryManager
  {
    void Start() override;
    void Stop(::benchmark::MemoryManager::Result* result) override;

    MicrobenchMemory::global_memory_informations start_;
  };

} // namespace MicrobenchMemory::fixtures::GoogleBenchmarkBridge
