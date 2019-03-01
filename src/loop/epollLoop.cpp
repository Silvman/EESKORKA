//
// Created by silvman on 3/1/19.
//

#include "epollLoop.h"

eeskorka::epollLoop::epollLoop(const eeskorka::serverConfig &config) : config(config), epfd(0), sd(0) {}

eeskorka::epollLoop::~epollLoop() {
    if (epfd > 0) {
        close(epfd);
    }
}

void eeskorka::epollLoop::setClientCallback(std::function<int(int)> callback) {
    clientCallback = std::move(callback);
}

int eeskorka::epollLoop::init(int sd_list) {
    sd = sd_list;

    epfd = epoll_create(config.maxClients);
    if (epfd == -1) {
        spdlog::critical(EPOLL_CREATE_ERR);
        return -1;
    }
    spdlog::info("create epoll, epfd {}", epfd);

    listenEvent.data.fd = sd;
    listenEvent.events = EPOLLIN | EPOLLEXCLUSIVE;
    // listen socket is nonblocking; using Level Triggered with EPOLLEXCLUSIVE to avoid thundering herd problem
    // on many connections and starvation when only one thread is processing all connections

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sd, &listenEvent) != 0) {
        spdlog::critical("epoll ctl failed");
        return -1;
    }
    spdlog::info("bind epoll to the socket");

    return 0;
}

std::string eeskorka::epollLoop::IPAddressToString(int ip) {
    return fmt::format("{}.{}.{}.{}",
                       (ip >> 24) & 0xFF,
                       (ip >> 16) & 0xFF,
                       (ip >> 8) & 0xFF,
                       (ip) & 0xFF);
}

int eeskorka::epollLoop::acceptClients() {
    sockaddr_in client{};
    socklen_t cli_len = sizeof(client);

    loop {
        int cli_sd = accept(sd, (struct sockaddr *) &client, &cli_len);
        if (cli_sd > 0) {
            if (fcntl(cli_sd, F_SETFL, fcntl(cli_sd, F_GETFL, 0) | O_NONBLOCK) == -1) {
                spdlog::critical("failure on fcntl");
                return -1;
            }

            epoll_event cli_ev{};
            cli_ev.data.fd = cli_sd;
            cli_ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
            // using Edge Triggered with EPOLLONESHOT to unblock epoll_wait only once;

            if (epoll_ctl(epfd, EPOLL_CTL_ADD, cli_sd, &cli_ev) == -1) {
                spdlog::warn("accpet, epoll_ctl: {}", strerror(errno));
            };

            spdlog::info("new client: sd {}, from {}", cli_sd,
                         IPAddressToString(client.sin_addr.s_addr));

        } else if (cli_sd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                spdlog::debug("done accept");
                break;
            }

            spdlog::critical("cli_sd == -1 && errno == {}", strerror(errno));
            return -1;
        } else {
            spdlog::debug("? cli_sd: {}", cli_sd);
            return -1;
        }
    }

    return 0;
}

std::function<void()> eeskorka::epollLoop::getEventLoop() {
    return [=]() {
        auto *events = new epoll_event[config.maxClients];

        loop {
            int numEvents = epoll_wait(epfd, events, config.maxClients, -1);

            for (int i = 0; i < numEvents; i++) {
                if (events[i].data.fd == sd) {
                    if (acceptClients() != 0) {
                        spdlog::critical("failure on accept clients");
                        goto failure;
                    }
                } else if (events[i].events & EPOLLIN) {
                    if (clientCallback(events[i].data.fd) == 0) {
                        // todo rearming socket for http/1.1
                        if (epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]) == -1) {
                            spdlog::warn("elentLoop, epoll_ctl, del: {}", strerror(errno));
                        }
                        close(events[i].data.fd);
                    } else {
                        spdlog::critical("failure on serving clients");
                        goto failure;
                    }
                }
            }
        }

        failure:
        delete[] events;
    };
}