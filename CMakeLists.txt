cmake_minimum_required (VERSION 3.1)

# Stuff for making coverage builds.
include("coverage.cmake")
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Project metadata
project (libstephen)
set (libstephen_VERSION_MAJOR 0)
set (libstephen_VERSION_MINOR 1)

# Source declarations.
include("src/CMakeLists.txt")
include("test/CMakeLists.txt")

# Add an include directory.
include_directories("inc")

# Declare targets and dependencies among them.
add_library(stephen SHARED ${libstephen_SOURCES})
add_executable(test_libstephen ${libstephen_TEST_SOURCES})
add_executable(regex util/regex.c)
target_link_libraries(test_libstephen stephen)
target_link_libraries(regex stephen)

# Tests (using CMake)
file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
enable_testing()
add_test(all_tests test_libstephen)

# Installation
install (TARGETS stephen DESTINATION lib)
install (DIRECTORY inc/libstephen DESTINATION include)
