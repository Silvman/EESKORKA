//
// Created by silvman on 3/1/19.
//

#include "EpollLoop.h"

eeskorka::epollLoop::epollLoop() : epfd(0), sd(0) {
    epollCallback = [&](int sd, connectionAction action) {
        epoll_event ev {};
        ev.data.fd = sd;

        switch (action) {
            case closeConnection: {
                if (epoll_ctl(epfd, EPOLL_CTL_DEL, sd, &ev) == -1) {
                    log(err, "elentLoop, epoll_ctl, del: {}", strerror(errno));
                }
                close(sd);

                break;
            }

            case rearmConnection: {
                ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
                if (epoll_ctl(epfd, EPOLL_CTL_MOD, sd, &ev) == -1) {
                    log(err, "elentLoop, epoll_ctl, mod: {}", strerror(errno));
                }

                break;
            }

            case waitUntillReaded: {
                ev.events = EPOLLOUT | EPOLLET | EPOLLONESHOT;
                if (epoll_ctl(epfd, EPOLL_CTL_MOD, sd, &ev) == -1) {
                    log(err, "elentLoop, epoll_ctl, mod: {}", strerror(errno));
                }

                break;
            }

            default: {
                log(warn, "unknown action");
            }
        }
    };
}

eeskorka::epollLoop::~epollLoop() {
    if (epfd > 0) {
        close(epfd);
    }
}

void eeskorka::epollLoop::setClientCallback(clientCallbackType callback) {
    clientCallback = std::move(callback);
}

int eeskorka::epollLoop::init(int sd_list) {
    sd = sd_list;

    epfd = epoll_create(config.maxClients);
    if (epfd == -1) {
        log(critical, EPOLL_CREATE_ERR);
        return -1;
    }
    log(info, "create epoll, epfd {}", epfd);

    listenEvent.data.fd = sd;
    listenEvent.events = EPOLLIN | EPOLLEXCLUSIVE;
    // listen socket is nonblocking; using Level Triggered with EPOLLEXCLUSIVE to avoid thundering herd problem
    // on many connections and starvation when only one thread is processing all connections

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sd, &listenEvent) != 0) {
        log(critical, "epoll ctl failed");
        return -1;
    }
    log(info, "bind epoll to the socket");

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
                log(critical, "failure on fcntl");
                return -1;
            }

            epoll_event cli_ev{};
            cli_ev.data.fd = cli_sd;
            cli_ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
            // using Edge Triggered with EPOLLONESHOT to unblock epoll_wait only once;

            if (epoll_ctl(epfd, EPOLL_CTL_ADD, cli_sd, &cli_ev) == -1) {
                log(warn, "accpet, epoll_ctl: {}", strerror(errno));
            };

            log(info, "new client: sd {}, from {}", cli_sd,
                       IPAddressToString(client.sin_addr.s_addr));

        } else if (cli_sd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                log(debug, "done accept");
                break;
            }

            log(critical, "cli_sd == -1 && errno == {}", strerror(errno));
            return -1;
        } else {
            log(debug, "? cli_sd: {}", cli_sd);
            return -1;
        }
    }

    return 0;
}

std::function<void()> eeskorka::epollLoop::getEventLoop() {
    return [&]() {
        auto *events = new epoll_event[config.maxClients];

        loop {
            int numEvents = epoll_wait(epfd, events, config.maxClients, -1);

            for (int i = 0; i < numEvents; i++) {
                if (events[i].data.fd == sd) {
                    if (acceptClients() != 0) {
                        log(critical, "failure on accept clients");
                        goto failure;
                    }
                } else if (events[i].events & (EPOLLIN)) {
                    if (clientCallback(events[i].data.fd, epollCallback) != 0) {
                        log(critical, "failure on serving clients");
                        goto failure;
                    }
                } else if (events[i].events & (EPOLLOUT)) {
                    log(info, "epollout");

                    if (clientCallback(events[i].data.fd, epollCallback) != 0) {
                        log(critical, "failure on serving clients");
                        goto failure;
                    }
                }
            }
        }

        failure:
        delete[] events;
    };
}
