project(test_gcc_inc_link)
cmake_minimum_required(VERSION 3.16)

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "CMAKE_CURRENT_LIST_FILE: ${CMAKE_CURRENT_LIST_FILE}")

message(STATUS "CMAKE_CURRENT_LIST_LINE: ${CMAKE_CURRENT_LIST_LINE}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc2)

file(GLOB_RECURSE HEAD_FILES *.hpp)
message(STATUS "HEAD_FILES: ${HEAD_FILES}")

add_subdirectory(src)
