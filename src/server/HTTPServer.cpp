//
// Created by silvman on 3/1/19.
//

#include "HTTPServer.h"
#include "../http/HTTPRequest.h"
#include "../http/HTTPResponse.h"
#include "../http/HTTPContext.h"
#include "../handlers/StaticHandler.h"

bool eeskorka::httpServer::isHeaderOver(const std::string &s) {
    return s.find("\r\n\r\n") != std::string::npos;
}

int eeskorka::httpServer::startStaticServer() {
    basicServer.setClientCallback(std::bind(&httpServer::onClientReady, this, std::placeholders::_1,
                                            std::placeholders::_2));
    int err = basicServer.start();
    return err;
}

int eeskorka::httpServer::onClientReady(int sd, loopCallbackType &loopCallback) {
    std::shared_ptr<HTTPContext> ptr;
    mtx.lock();
    if (clients.find(sd) == clients.end()) {
        clients[sd] = std::make_shared<HTTPContext>(sd);
    }
    ptr = clients[sd];
    mtx.unlock();

    auto &httpContext = *(ptr);

    if (!httpContext.hasUnfinishedTask()) {
        std::string rawMessage;
        if (readFromSocket(sd, rawMessage) == 0) {
            if (isHeaderOver(rawMessage)) {
                httpContext.request.parseRawHeader(rawMessage);
                httpContext.response.statusLine.http_version = httpContext.request.requestLine.http_version;

                try {
                    handleStatic(httpContext);
                } catch (const std::exception &e) {
                    log(err, e.what());
                }

                httpContext.writeHeader();
                httpContext.writeFile();
            }
        }
    } else {
        httpContext.resumeTask();
    }

    if (!httpContext.hasUnfinishedTask()) {
        mtx.lock();
        clients.erase(sd);
        mtx.unlock();
        loopCallback(sd, closeConnection);
    } else {
        loopCallback(sd, waitUntillReaded);
    }

    return 0;
}

int eeskorka::httpServer::readFromSocket(int sd, std::string &raw) {
    char buffer[config.bufferSize];

    loop {
        ssize_t n = read(sd, buffer, config.bufferSize);

        if (n > 0) {
            raw.append(buffer, n);
        } else {
            if (n == -1) {
                // прочитали сообщение
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                } else {
                    return -1;
                }
            } else {
                return 0;
            }
        }
    };

    return 0;
}

eeskorka::httpServer::httpServer() {
    clients.reserve(static_cast<unsigned long>(config.maxClients * config.workers / clients.max_load_factor()));
}

eeskorka::httpServer::~httpServer() {
}
