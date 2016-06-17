cmake_minimum_required(VERSION 3.5)
project(client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES client.c client.h)
add_executable(client ${SOURCE_FILES})