[![Actions Status](https://github.com/dutiona/MicrobenchMemory/workflows/MacOS/badge.svg)](https://github.com/dutiona/MicrobenchMemory/actions)
[![Actions Status](https://github.com/dutiona/MicrobenchMemory/workflows/Windows/badge.svg)](https://github.com/dutiona/MicrobenchMemory/actions)
[![Actions Status](https://github.com/dutiona/MicrobenchMemory/workflows/Ubuntu/badge.svg)](https://github.com/dutiona/MicrobenchMemory/actions)
[![codecov](https://codecov.io/gh/dutiona/MicrobenchMemory/branch/master/graph/badge.svg)](https://codecov.io/gh/dutiona/MicrobenchMemory)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/dutiona/MicrobenchMemory)](https://github.com/dutiona/MicrobenchMemory/releases)

# Microbench Memory

Microbench Memory is a library utility that benchmark the memory usage/consumption
at when running code. The library works by intercepting calls to global:

```cpp
::operator new
::operator delete
```

## Core Features

The library provides two macros. One allows global interception for the globality of the program:

```
// put this on global scope
MICROBENCH_MEMORY_OVERLOAD_GLOBAL()
```

All the calls to new and delete will be instrumented and statistics will be kept in a global static structure whose definition is:

```cpp
struct global_memory_informations
{
  std::size_t allocation_count;
  std::size_t deallocation_count;
  std::size_t total_memory_allocated;

  bool has_memory_leak() const { return allocation_count != deallocation_count; }
};
```

It is possible to get those statistics at any points in the program by calling:

```cpp
auto mem_infos = MicrobenchMemory::get_global_memory_information_snapshot();
```

This structure support the + and - operator overload so that it is easy for the user to make a difference between 2 points in the program.

In addition, the library support more in-depth instrumentation when plugged into a structure.
You can instrument a structure by introducing the following code in it:

```cpp
struct instrumented_A
{
  int a, b, c;
  MICROBENCH_MEMORY_INSTRUMENT_CLASS("instrumented_A");
};
```

This will allow the program to keep track of every new/delete related to this structure by injecting static new/delete overload inside.
The user can then gather related information to the memory usage of this structure inside the program by calling:

```cpp
auto mem_infos = MicrobenchMemory::get_memory_information_snapshot("instrumented_A");
```

The returned data structure will be more detailed than the global one:

```cpp
struct scoped_memory_informations
{
  std::size_t      allocation_count;
  std::size_t      deallocation_count;
  std::size_t      total_memory_allocated;
  std::size_t      total_memory_deallocated;
  std::size_t      nb_ptr_leaked;
  std::string_view scope;

  bool        has_memory_leak() const { return allocation_count != deallocation_count || total_memory_allocated != total_memory_deallocated; }
  std::size_t count_memory_leaked() const { return total_memory_allocated - total_memory_deallocated; }
  std::size_t count_ptr_leaked() const { return nb_ptr_leaked; }

  operator global_memory_informations() const { return {allocation_count, deallocation_count, total_memory_allocated}; }
};
```
The conversion facility is provided for ease of use when used inside operator - or + to deduce, for instance, how % of the program is used by a specific data structure.

## Extension IO

Additional facilities are provided inside the io.hpp header to output the aforementioned data structure onto a output stream.

```cpp
auto mem_infos_g = MicrobenchMemory::get_global_memory_information_snapshot();
std::cout << mem_infos_g;

auto mem_infos_scoped_a = MicrobenchMemory::get_memory_information_snapshot("instrumented_A");
std::cout << mem_infos_scoped_a;
```

## Fixture included : bridge to google benchmark

The final point of this library is to provide a bridge to plug to google benchmark.
Indeed, google benchmark include a MemoryManager interface the user can use to populate information about memory usage.
This library extends this memory manager and populate it with the difference of memory consumption between two points in time.

Here is how to use it:

```cpp
#include "MicrobenchMemory/MicrobenchMemory.hpp"
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

```

## Getting Started

Simply install the library via cmake --build <build_dir> --target install and link against it.

### Prerequisites

This projects has 2 dependencies:
* google test
* google benchmark

## Versioning

This project makes use of [SemVer](http://semver.org/) for versioning. A list of
existing versions can be found in the
[project's releases](https://github.com/dutiona/MicrobenchMemory/releases).

## Authors

* **Michaël ROYNARD** - [@dutiona](https://github.com/dutiona)

## License

This project is licensed under the [MIT](https://en.wikipedia.org/wiki/MIT_License) - see the
[LICENSE](LICENSE) file for details
