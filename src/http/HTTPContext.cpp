//
// Created by silvman on 3/2/19.
//

#include "HTTPContext.h"

int eeskorka::HTTPContext::writeCompletely(const char *buffer, size_t size) {
    ssize_t written = 0, nn = 0;

    while (written != size) {
        nn = write(sd, buffer, size);
        if (nn == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                logger.log(debug, "writeCompletely: EAGAIN");
                return 0;
            }

            return -1;
        }

        written += nn;
    }

    return 0;
}

eeskorka::HTTPContext::HTTPContext(int sd, eeskorka::HTTPRequest &req, eeskorka::HTTPResponse &resp, serverConfig &cfg,
                                   loopCallbackType& cb)
        : sd(sd),
          request(req),
          response(resp),
          logger(ServerLogger::get()),
          config(cfg),
          loopCallback(cb) { }

int eeskorka::HTTPContext::writeFile(const std::filesystem::path &p) {
    auto sizeLeft = fs::file_size(p);

    char buffer[config.bufferSize]; // todo use config

    std::ifstream fReader;
    fReader.open(p);

    int err = 0;
    while (sizeLeft != 0) {
        fReader.read(buffer, config.bufferSize);

        if (sizeLeft >= config.bufferSize) {
            err = writeCompletely(buffer, config.bufferSize);
            sizeLeft -= config.bufferSize;
        } else {
            err = writeCompletely(buffer, sizeLeft);
            sizeLeft = 0;
        }

        if (err != 0) {
            logger.log(critical, "writecompletely fail");
            break;
        }
    }

    return -1;
}

int eeskorka::HTTPContext::writeHeader() {
    std::string responseHeader = response.getHeader();
    return writeCompletely(responseHeader.c_str(), responseHeader.length());
}

int eeskorka::HTTPContext::writeBody(const char *buffer, size_t size) {
    return writeCompletely(buffer, size);
}

void eeskorka::HTTPContext::close() {
    loopCallback(sd, closeConnection);
}
