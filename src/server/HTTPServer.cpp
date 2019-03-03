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
    HTTPRequest httpRequest;
    std::string rawMessage;

    if (readFromSocket(sd, rawMessage) == 0) {
        if (isHeaderOver(rawMessage)) {
            logger.log(info, "header is read");

            httpRequest.parseRawHeader(rawMessage);

            HTTPResponse httpResponse;
            httpResponse.statusLine.http_version = httpRequest.requestLine.http_version;

            // responsibility is going to context
            HTTPContext httpContext(sd, httpRequest, httpResponse, config, loopCallback);

            // there should be dispatch...
            try {
                handleStatic(httpContext);
            } catch (const std::exception &e) {
                logger.log(err, e.what());
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
