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
    if (clients.find(sd) != clients.end()) {
        auto& httpContext = *(clients[sd]);
        httpContext.resumeTask();

    } else {
        clients.emplace(sd, std::make_shared<HTTPContext>(sd));
        auto& httpContext = *(clients[sd]);

        std::string rawMessage;
        if (readFromSocket(sd, rawMessage) == 0) {
            if (isHeaderOver(rawMessage)) {
                eeskorka::log(info, "header is read");

                httpContext.request.parseRawHeader(rawMessage);
                httpContext.response.statusLine.http_version = httpContext.request.requestLine.http_version;

                try {
                    handleStatic(httpContext);
                } catch (const std::exception &e) {
                    log(err, e.what());
                }

                if (!httpContext.hasUnfinishedTask()) {
                    clients.erase(sd);
                    loopCallback(sd, closeConnection);
                } else {
                    loopCallback(sd, waitUntillReaded);
                }

                return 0;
            } else {
                log(warn, "header is broken");
            }
        }

        return 0;
    }

    return 0;
}

int eeskorka::httpServer::readFromSocket(int sd, std::string &raw) {
    char buffer[config.bufferSize];

    loop {
        ssize_t n = read(sd, buffer, config.bufferSize);
        log(debug, "n: {}", n);

        if (n > 0) {
            raw.append(buffer, n);
        } else {
            if (n == -1) {
                // прочитали сообщение
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    log(debug, "read message, errno eagain");
                    break;
                } else {
                    log(critical, "read message, errno: {}", strerror(errno));
                    return -1;
                }
            } else {
                log(debug, "n=0, disconnect");
                return 0;
            }
        }
    };

    return 0;
}
