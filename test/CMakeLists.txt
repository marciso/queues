if(USE_GTEST)
add_executable(queues_test_app
    mpmc_bounded_fifo_test.cpp
    spsc_bounded_fifo_test.cpp
	mpmc_naive_fifo_test.cpp)

target_link_libraries(queues_test_app
    queues
	libgmock_main
	libgtest
	libgmock
    )

add_test(NAME queues_test
	COMMAND queues_test_app)
endif()

if(USE_BENCHMARK)
add_executable(benchmark_test_app
    benchmark_test.cpp)

target_link_libraries(benchmark_test_app
    queues
    benchmark
    benchmark_main)

add_test(NAME benchmark_test
	COMMAND benchmark_test_app)
endif()

