#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <thread>
#include <csignal>
#include <sstream>
#include "spdlog/spdlog.h"
#include "server/httpServer.h"
#include <filesystem>
#include <fstream>
#include <netinet/in.h>


int main() {
    spdlog::set_pattern("[%H:%M:%S %z] [%^%l%$] [thread %t] %v");
    spdlog::set_level(spdlog::level::debug);

    // config reading
    eeskorka::serverConfig config;

    eeskorka::httpServer server(config);
    if (server.startStaticServer() == -1) {
        spdlog::critical("cannot start server");
    }

    return 0;
}
