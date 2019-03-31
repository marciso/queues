
add_library(queues STATIC
    spsc_bounded_fifo.hpp
    mpmc_naive_fifo.hpp
    mpmc_bounded_fifo.hpp
    priority_queue_bounded.hpp
    utils.hpp
    utils.cpp
)

# for header only library: add_library(queues INTERFACE project_name.hpp)

target_include_directories(queues
	INTERFACE .)

add_executable(queues_app
    main.cpp
)

target_link_libraries(queues_app
    queues)
