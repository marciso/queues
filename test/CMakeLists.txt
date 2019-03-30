add_executable(queues_test_app
	queues_test.cpp)

target_link_libraries(queues_test_app
    queues
	libgmock_main
	libgtest
	libgmock
    )

add_test(NAME queues_test
	COMMAND queues_test_app)

