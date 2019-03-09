//
// Created by silvman on 3/1/19.
//

#ifndef EESKORKA_SERVER_H
#define EESKORKA_SERVER_H

#include <thread>
#include <vector>
#include <functional>
#include "ServerConfig.h"
#include "../loop/EpollLoop.h"
#include "../logger/ServerLogger.h"
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <netinet/in.h>

namespace eeskorka {
    class server {
    private:
        std::vector<std::thread> loops;
        epollLoop multiplexer;

        int sd;
        sockaddr_in serv_addr;

        int createListeningSocket();

    public:
        server();
        virtual ~server();

        void setClientCallback(clientCallbackType callback);
        int start();
    };
}

#endif //EESKORKA_SERVER_H
