//
// Created by silvman on 3/1/19.
//

#ifndef EESKORKA_SERVERCONFIG_H
#define EESKORKA_SERVERCONFIG_H

#include <filesystem>

namespace eeskorka {
    namespace fs = std::filesystem;

    struct serverConfig {
        int numCores;
        int numThreads;
        int port;
        int maxClients;
        size_t bufferSize;
//        std::string addres;
//        fs::path rootDir;

        serverConfig() :
                numCores(4),
                numThreads(256),
                port(5000),
                maxClients(1024),
                bufferSize(1024) { }
    };
}


#endif //EESKORKA_SERVERCONFIG_H