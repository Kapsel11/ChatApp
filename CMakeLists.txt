cmake_minimum_required(VERSION 3.18)
project(ChatApp)

set(CMAKE_CXX_STANDARD 14)

add_executable(server server.cpp server.h)
add_executable(client client.cpp client.h)
target_link_libraries(ChatApp pthread)