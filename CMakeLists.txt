cmake_minimum_required(VERSION 3.13)

project(highload_silvman)

set(CMAKE_C_STANDARD 99)

include_directories(include)
link_directories(lib)

add_executable(highload_silvman src/main.c)
target_link_libraries(highload_silvman libuv.a pthread)