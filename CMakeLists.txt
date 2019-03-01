cmake_minimum_required(VERSION 3.12)

project(eeskorka)

set(CMAKE_CXX_STANDARD 17)

include_directories(vendor/spdlog/include)

add_executable(eeskorka src/main.cpp src/loop/EpollLoop.cpp src/loop/EpollLoop.h src/server/Server.cpp src/server/Server.h src/server/ServerConfig.cpp src/server/ServerConfig.h src/server/HTTPServer.cpp src/server/HTTPServer.h src/http/HTTPResponse.cpp src/http/HTTPResponse.h src/http/HTTPRequest.cpp src/http/HTTPRequest.h src/logger/ServerLogger.h src/http/HTTPContext.cpp src/http/HTTPContext.h src/http/HTTPUtility.h src/http/HTTPUtility.cpp src/handlers/StaticHandler.h src/handlers/StaticHandler.cpp)
target_link_libraries(eeskorka pthread stdc++fs)
