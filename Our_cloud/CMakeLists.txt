cmake_minimum_required(VERSION 3.5)
project(Our_cloud)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
find_package (Threads)

set(SOURCE_FILES run.c core/cloud.h core/networking.h core/server.h core/all_functions.h core/files.h core/cloud_interface.h core/storage.h)

add_executable(Our_cloud ${SOURCE_FILES})
target_link_libraries (Our_cloud ${CMAKE_THREAD_LIBS_INIT})