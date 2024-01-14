cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 17)
project(lab5)

add_subdirectory("src_lib")


find_package(Boost REQUIRED COMPONENTS system)

add_executable(client client.cpp json.hpp)
target_link_libraries(client src_lib ${Boost_LIBRARIES})

add_executable(server server.cpp json.hpp)
target_link_libraries(server src_lib ${Boost_LIBRARIES})