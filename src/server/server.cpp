//
// Created by silvman on 3/1/19.
//

#include "server.h"

eeskorka::server::server(const eeskorka::serverConfig &config) : config(config), multiplexer(config), sd(0) {
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // todo address
    serv_addr.sin_port = htons(config.port);
}

eeskorka::server::~server() {
    if (!loops.empty()) {
        for (auto &thread : loops) {
            thread.join();
        }
    }

    if (sd > 0) {
        close(sd);
    }
}

void eeskorka::server::setClientCallback(std::function<int(int)> callback) {
    multiplexer.setClientCallback(std::move(callback));
}

int eeskorka::server::createListeningSocket() {
    spdlog::info("server is starting");

    // creating socket
    sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd == -1) {
        spdlog::critical(SOCK_CREATE_ERR);
        return 1;
    }
    spdlog::info("open socket, sd {}", sd);

    // set non-blocking
    if (fcntl(sd, F_SETFL, fcntl(sd, F_GETFL, 0) | O_NONBLOCK) == -1) {
        spdlog::critical("failure on fcntl");
    }

    // binding
    if (bind(sd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        spdlog::critical("bind failed");
        return 1;
    }
    spdlog::info("bind port {} to the socket", config.port);

    // set socket listening
    if (listen(sd, config.maxClients) != 0) {
        spdlog::critical("listen failed");
        return 1;
    }
    spdlog::info("start listening for new connections");

    return 0;
}

int eeskorka::server::start() {
    if (createListeningSocket() != 0) {
        spdlog::critical("createListeningSocket failed");
        return 1;
    }

    if (multiplexer.init(sd) != 0) {
        spdlog::critical("multiplexer.init failed");
        return 1;
    };

    for (int i = 0; i < config.numCores - 1; ++i) {
        loops.emplace_back(multiplexer.getEventLoop());
    }

    multiplexer.getEventLoop()();

    return 0;
}
