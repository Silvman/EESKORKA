//
// Created by silvman on 3/1/19.
//

#ifndef EESKORKA_SERVERCONFIG_H
#define EESKORKA_SERVERCONFIG_H

#include <filesystem>

namespace eeskorka {
    namespace fs = std::filesystem;

    struct serverConfig {
        int numCores {4};
        int numThreads {256};
        int port {80};
        int maxClients {1024};
        size_t bufferSize {1024};
        std::string rootDir {"."};

        serverConfig() = default;
        explicit serverConfig(const fs::path& p);

    };
}


#endif //EESKORKA_SERVERCONFIG_H
