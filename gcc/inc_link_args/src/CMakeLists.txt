file(GLOB_RECURSE SRC_FILES *.cpp)
message(STATUS "SRC_FILES: ${SRC_FILES}")
add_executable(test_gcc_inc_link ${SRC_FILES})