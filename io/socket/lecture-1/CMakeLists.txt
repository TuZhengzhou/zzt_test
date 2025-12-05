project(socket)

add_executable(
    client1
    client1.cpp
)

add_executable(
    server1
    server1.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)