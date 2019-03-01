//
// Created by silvman on 3/1/19.
//

#ifndef EESKORKA_HTTPSERVER_H
#define EESKORKA_HTTPSERVER_H
#define DEBUG

#include <functional>
#include "server.h"
#include <filesystem>
#include <fstream>

namespace eeskorka {

    struct RequestLine {
        std::string method;
        std::string request_uri;
        std::string http_version;
    };

// idk wat im doin
    std::string toLowerCase(std::string str);

    struct HTTPMessage {
        RequestLine requestLine;
        std::unordered_map<std::string, std::string> headers;
//    std::string body;

        void parseRawHeader(const std::string &raw);
    };

    /**
     * задачи:
     * предоставить колбэк для обработки пришедшего клиента
     * прочитать заголовок
     *
     * в функции статического севрера:
     * проверить метод, запрос, составить ответ и записать в сокет
     * при возникновении ошибок вернуть ошибку в loop для последующей обработки
     */

    class httpServer {
    private:
        server basicServer;
        serverConfig config;

        bool isHeaderOver(const std::string &s);

        int writeCompletely(int fd, const char *buffer, size_t size);

    public:
        explicit httpServer(const serverConfig &config) : config(config), basicServer(config) {}

        int startStaticServer();
        int onClientReady(int sd);
    };
}


#endif //EESKORKA_HTTPSERVER_H
