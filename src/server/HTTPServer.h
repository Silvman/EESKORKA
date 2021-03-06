//
// Created by silvman on 3/1/19.
//

#ifndef EESKORKA_HTTPSERVER_H
#define EESKORKA_HTTPSERVER_H
#define DEBUG

#include <functional>
#include "Server.h"
#include "../http/HTTPContext.h"
#include "../http/HTTPUtility.h"
#include <fstream>

namespace eeskorka {
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

        bool isHeaderOver(const std::string &s);
        int readFromSocket(int sd, std::string& raw);

        std::unordered_map<int, std::shared_ptr<HTTPContext>> clients;
        std::mutex mtx;

    public:
        httpServer();
        virtual ~httpServer();

        int startStaticServer();
        int onClientReady(int sd, loopCallbackType& loopCallback);

    };

}


#endif //EESKORKA_HTTPSERVER_H
