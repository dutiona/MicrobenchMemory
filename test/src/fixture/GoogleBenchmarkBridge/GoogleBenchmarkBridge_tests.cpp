#include "MicrobenchMemory/MicrobenchMemory.hpp"
#include "MicrobenchMemory/ext/io.hpp"
#include "MicrobenchMemory/fixture/GoogleBenchmarkBridge/GoogleBenchmarkBridge.hpp"

#include <benchmark/benchmark.h>

#include <memory>

void BM_empty(benchmark::State& state)
{
  for (auto _ : state)
  {
    benchmark::DoNotOptimize(state.iterations());
  }
}
BENCHMARK(BM_empty);


int main(int argc, char* argv[])
{
  std::unique_ptr<::benchmark::MemoryManager> mm(new MicrobenchMemory::fixtures::GoogleBenchmarkBridge::MemoryManager());

  ::benchmark::RegisterMemoryManager(mm.get());
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::RegisterMemoryManager(nullptr);
}
