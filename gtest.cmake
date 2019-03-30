
# We need thread support
find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install GoogleTest
ExternalProject_Add(
	gtest
	URL "https://github.com/google/googletest/archive/release-1.8.1.tar.gz"
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
	# Disable install step
	INSTALL_COMMAND ""
	LOG_DOWNLOAD ON
	LOG_CONFIGURE ON
	LOG_BUILD ON
	)

# Get GTest source and binary directories from CMake
# project
ExternalProject_Get_Property(gtest source_dir binary_dir)

file(MAKE_DIRECTORY "${source_dir}/googletest/include")
file(MAKE_DIRECTORY "${source_dir}/googlemock/include")

add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)
set_target_properties(libgtest PROPERTIES
	IMPORTED_LOCATION "${binary_dir}/googlemock/gtest/libgtest.a"
	INTERFACE_INCLUDE_DIRECTORIES "${source_dir}/googletest/include"
	IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
	)

add_library(libgtest_main IMPORTED STATIC GLOBAL)
add_dependencies(libgtest_main gtest)
set_target_properties(libgtest_main PROPERTIES
	IMPORTED_LOCATION "${binary_dir}/googlemock/gtest/libgtest_main.a"
	INTERFACE_INCLUDE_DIRECTORIES "${source_dir}/googletest/include"
	IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
	)

add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)
set_target_properties(libgmock PROPERTIES
	IMPORTED_LOCATION "${binary_dir}/googlemock/libgmock.a"
	INTERFACE_INCLUDE_DIRECTORIES "${source_dir}/googlemock/include"
	IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
	)

add_library(libgmock_main IMPORTED STATIC GLOBAL)
add_dependencies(libgmock_main gtest)
set_target_properties(libgmock_main PROPERTIES
	IMPORTED_LOCATION "${binary_dir}/googlemock/libgmock_main.a"
	INTERFACE_INCLUDE_DIRECTORIES "${source_dir}/googlemock/include"
	IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
	)

