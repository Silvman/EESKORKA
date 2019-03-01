#include <utility>

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
#include <netinet/in.h>

namespace eeskorka {

    /**
     * сервер принимает конфиг в виде объекта
     *
     * задача:
     * создать сокет
     * забиндить
     * слушать
     * запустить эвент луп
     *
     */

    const char SOCK_CREATE_ERR[] = "Failed to create socket descriptor\n";
    const char SOCK_CLOSE_ERR[] = "Failed to close socket descriptor\n";

    class server {
    private:
        serverConfig config;
        std::vector<std::thread> loops;
        epollLoop multiplexer;

        int sd;
        sockaddr_in serv_addr;

        int createListeningSocket();

        ServerLogger& logger;

    public:
        explicit server(const eeskorka::serverConfig &config);
        virtual ~server();

        void setClientCallback(std::function<int(int)> callback);
        int start();
    };
}

#endif //EESKORKA_SERVER_H
