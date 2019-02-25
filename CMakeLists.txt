cmake_minimum_required(VERSION 3.12)

project(highload_silvman)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
link_directories(lib)

add_executable(highload_silvman src/main.cpp)
target_link_libraries(highload_silvman pthread stdc++fs)
