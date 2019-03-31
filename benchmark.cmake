
# We need thread support
find_package(Threads REQUIRED)

include(ExternalProject)

ExternalProject_Add(google_benchmark
  URL "https://github.com/google/benchmark/archive/v1.4.1.tar.gz"
  #GIT_REPOSITORY https://github.com/google/benchmark.git
  #GIT_TAG v1.4.1
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/google_benchmark
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON
  CMAKE_ARGS
    -DBENCHMARK_ENABLE_TESTING=OFF
    -DBENCHMARK_ENABLE_INSTALL=OFF
    -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
    -DCMAKE_BUILD_TYPE=Release
  INSTALL_COMMAND ""
 )

ExternalProject_Get_Property(google_benchmark source_dir binary_dir)

file(MAKE_DIRECTORY "${source_dir}/include")

add_library(benchmark IMPORTED STATIC GLOBAL)
add_dependencies(benchmark google_benchmark)
set_target_properties(benchmark PROPERTIES
	IMPORTED_LOCATION "${binary_dir}/src/libbenchmark.a"
	INTERFACE_INCLUDE_DIRECTORIES "${source_dir}/include"
	IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
	)

add_library(benchmark_main IMPORTED STATIC GLOBAL)
add_dependencies(benchmark_main google_benchmark)
set_target_properties(benchmark_main PROPERTIES
	IMPORTED_LOCATION "${binary_dir}/src/libbenchmark_main.a"
	INTERFACE_INCLUDE_DIRECTORIES "${source_dir}/include"
	IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
	)

