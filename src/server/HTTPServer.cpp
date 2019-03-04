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
        clients[sd].writeFile();

        if (clients[sd].hasUnfinishedTask()) {
            loopCallback(sd, rearmConnection);
        } else {
            loopCallback(sd, closeConnection);
        }

        return 0;
    }

    clients.emplace(sd, HTTPContext(sd, config, loopCallback));
    HTTPContext& httpContext = clients[sd];
    std::string rawMessage;

    if (readFromSocket(sd, rawMessage) == 0) {
        if (isHeaderOver(rawMessage)) {
            logger.log(info, "header is read");

            httpContext.request.parseRawHeader(rawMessage);
            httpContext.response.statusLine.http_version = httpContext.request.requestLine.http_version;

            try {
                handleStatic(httpContext);
            } catch (const std::exception &e) {
                logger.log(err, e.what());
            }

            if (!httpContext.hasUnfinishedTask()) {
                clients.erase(sd);
            } else {
                loopCallback(sd, rearmConnection);
            }

            return 0;
        } else {
            logger.log(warn, "header is broken");
        }
    }

    loopCallback(sd, closeConnection);
    return 0;
}

int eeskorka::httpServer::readFromSocket(int sd, std::string &raw) {
    char buffer[config.bufferSize];

    loop {
        ssize_t n = read(sd, buffer, config.bufferSize);
        logger.log(debug, "n: {}", n);

        if (n > 0) {
            raw.append(buffer, n);
        } else {
            if (n == -1) {
                // прочитали сообщение
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    logger.log(debug, "read message, errno eagain");
                    break;
                } else {
                    logger.log(critical, "read message, errno: {}", strerror(errno));
                    return -1;
                }
            } else {
                logger.log(debug, "n=0, disconnect");
                return 0;
            }
        }
    };

    return 0;
}
