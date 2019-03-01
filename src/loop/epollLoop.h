#include <utility>

//
// Created by silvman on 3/1/19.
//

#ifndef EESKORKA_EPOLL_LOOP_H
#define EESKORKA_EPOLL_LOOP_H

#include <functional>
#include <utility>
#include <sys/epoll.h>
#include <netinet/in.h>
#include "../server/serverConfig.h"
#include "../logger/serverLogger.h"

#define loop for(;;)

namespace eeskorka {
    const char EPOLL_CREATE_ERR[] = "Failed to create epoll file descriptor\n";
    const char EPOLL_CLOSE_ERR[] = "Failed to close epoll file descriptor\n";


    class epollLoop {
    public:
        explicit epollLoop(const serverConfig &config);
        virtual ~epollLoop();

        void setClientCallback(std::function<int(int)> callback);
        int init(int sd_list);
        std::function<void()> getEventLoop();

    private:
        std::function<int(int)> clientCallback;
        serverConfig config;

        epoll_event listenEvent{};
        int epfd;
        int sd;

        std::string IPAddressToString(int ip);
        int acceptClients();

        serverLogger& logger;
    };
}


#endif //EESKORKA_EPOLL_LOOP_H
