cmake_minimum_required(VERSION 3.12)

project(eeskorka)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vendor/boost_1_69_0")
#add_subdirectory(vendor/cds-2.3.2)

include_directories(vendor/spdlog/include)
#include_directories(cds)

add_executable(eeskorka src/main.cpp src/loop/EpollLoop.cpp src/loop/EpollLoop.h src/server/Server.cpp src/server/Server.h src/server/ServerConfig.cpp src/server/ServerConfig.h src/server/HTTPServer.cpp src/server/HTTPServer.h src/http/HTTPResponse.cpp src/http/HTTPResponse.h src/http/HTTPRequest.cpp src/http/HTTPRequest.h src/logger/ServerLogger.h src/http/HTTPContext.cpp src/http/HTTPContext.h src/http/HTTPUtility.h src/http/HTTPUtility.cpp src/handlers/StaticHandler.h src/handlers/StaticHandler.cpp src/types.h src/RuntimeArguments.cpp src/RuntimeArguments.h)
target_link_libraries(eeskorka pthread stdc++fs)
