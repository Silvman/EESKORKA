cmake_minimum_required(VERSION 3.12)

project(eeskorka)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
link_directories(lib)

add_executable(eeskorka src/main.cpp src/loop/epollLoop.cpp src/loop/epollLoop.h src/server/server.cpp src/server/server.h src/server/serverConfig.cpp src/server/serverConfig.h src/server/httpServer.cpp src/server/httpServer.h src/http/HTTPResponse.cpp src/http/HTTPResponse.h src/http/HTTPRequest.cpp src/http/HTTPRequest.h src/logger/serverLogger.h)
target_link_libraries(eeskorka pthread stdc++fs)
