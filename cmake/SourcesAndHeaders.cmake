set(sources
    src/MicrobenchMemory.cpp
)

set(headers
    include/MicrobenchMemory/MicrobenchMemory.hpp
    include/MicrobenchMemory/ext/io.hpp
)

set(test_sources
  src/MicrobenchMemory_tests.cpp
)


if(${PROJECT_NAME}_ENABLE_GOOGLEBENCHMARK_FIXTURE)
  message("Appending GoogleBenchmarkBridge fixture sources")
  list(APPEND sources
    src/fixture/GoogleBenchmarkBridge/GoogleBenchmarkBridge.cpp
  )
  list(APPEND headers
    include/fixture/GoogleBenchmarkBridge/GoogleBenchmarkBridge.hpp
  )

  list(APPEND test_sources
    src/fixture/GoogleBenchmarkBridge/GoogleBenchmarkBridge_tests.cpp
  )
endif()

set(exe_sources
		src/MicrobenchMemory.cpp
		${sources}
)