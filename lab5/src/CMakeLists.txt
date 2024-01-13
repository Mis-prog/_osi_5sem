cmake_minimum_required(VERSION 3.22.1)

project(lab5)

add_subdirectory("src_lib")

add_executable(client client.cpp)
target_link_libraries(client src_lib)

add_executable(server server.cpp)
target_link_libraries(server src_lib)