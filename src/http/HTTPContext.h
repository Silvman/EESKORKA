//
// Created by silvman on 3/2/19.
//

#ifndef EESKORKA_HTTPCONTEXT_H
#define EESKORKA_HTTPCONTEXT_H

#include "../http/HTTPRequest.h"
#include "../http/HTTPResponse.h"
#include "../logger/ServerLogger.h"
#include "../server/ServerConfig.h"
#include <filesystem>
#include <fstream>

namespace eeskorka {
    namespace fs = std::filesystem;

    class HTTPContext {
    public:
        HTTPContext(int sd, HTTPRequest &req, HTTPResponse &resp, serverConfig &cfg);

        HTTPRequest &request;
        HTTPResponse &response;
        const serverConfig& config;

        int writeHeader();
        int writeFile(const fs::path &p);
        int writeBody(const char *buffer, size_t size);

    private:
        int sd;
        int writeCompletely(const char *buffer, size_t size);

        ServerLogger &logger;
    };
}


#endif //EESKORKA_HTTPCONTEXT_H
