//
// Created by silvman on 3/2/19.
//

#ifndef EESKORKA_HTTPCONTEXT_H
#define EESKORKA_HTTPCONTEXT_H

#include "../http/HTTPRequest.h"
#include "../http/HTTPResponse.h"
#include "../logger/ServerLogger.h"
#include "../server/ServerConfig.h"
#include "../types.h"
#include <filesystem>
#include <fstream>

namespace eeskorka {
    namespace fs = std::filesystem;

    class HTTPContext {
    public:
        HTTPContext();
        HTTPContext(int sd, serverConfig &cfg, loopCallbackType& cb);
        virtual ~HTTPContext();
        HTTPContext(HTTPContext&& c) noexcept;

        HTTPRequest request;
        HTTPResponse response;
        const serverConfig& config;

        int writeHeader();
        int writeFile(const fs::path &p);
        int writeFile() {
            return this->writeFile(p);
        }
        int writeBody(const char *buffer, size_t size);

        bool hasUnfinishedTask();

        void close();

    private:
        bool unfinishedTask;
        int sd;
        int writeCompletely(const char *buffer, size_t size);

        ssize_t written {0}, nn {0};
        uintmax_t sizeLeft {0};
        std::filesystem::path p;
        char *buffer;
        std::ifstream fReader;


        loopCallbackType& loopCallback;
        ServerLogger &logger;
    };
}


#endif //EESKORKA_HTTPCONTEXT_H
