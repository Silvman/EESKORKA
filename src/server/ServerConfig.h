//
// Created by silvman on 3/1/19.
//

#ifndef EESKORKA_SERVERCONFIG_H
#define EESKORKA_SERVERCONFIG_H

#include <filesystem>

namespace eeskorka {
    namespace fs = std::filesystem;

    struct serverConfig {
        int workers {2};
        int numThreads {256};
        int port {80};
        int maxClients {256}; // per worker
        size_t bufferSize {1024};
        std::string rootDir {"."};

        serverConfig() = default;
        int readConfigFile(const std::filesystem::path &p);
    };


    extern serverConfig config;
}


#endif //EESKORKA_SERVERCONFIG_H
