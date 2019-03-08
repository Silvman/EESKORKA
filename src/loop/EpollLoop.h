//
// Created by silvman on 3/1/19.
//

#ifndef EESKORKA_EPOLL_LOOP_H
#define EESKORKA_EPOLL_LOOP_H

#include <utility>
#include <sys/epoll.h>
#include <netinet/in.h>
#include "../server/ServerConfig.h"
#include "../logger/ServerLogger.h"
#include "../types.h"

#define loop for(;;)

namespace eeskorka {
    const char EPOLL_CREATE_ERR[] = "Failed to create epoll file descriptor\n";
    const char EPOLL_CLOSE_ERR[] = "Failed to close epoll file descriptor\n";

    class epollLoop {
    public:
        explicit epollLoop();
        virtual ~epollLoop();

        void setClientCallback(clientCallbackType callback);
        int init(int sd_list);
        std::function<void()> getEventLoop();

    private:
        clientCallbackType clientCallback;
        loopCallbackType epollCallback;

        epoll_event listenEvent{};
        int epfd;
        int sd;

        std::string IPAddressToString(int ip);
        int acceptClients();
    };
}


#endif //EESKORKA_EPOLL_LOOP_H
