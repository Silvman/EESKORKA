//
// Created by silvman on 3/2/19.
//

#include "HTTPContext.h"

int eeskorka::HTTPContext::writeCompletely(const char *buffer, size_t size) {
    while (written != size) {
        nn = write(sd, buffer, size);
        if (nn == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                return -2;
            }

            return -1;
        }

        written += nn;
    }

    return 0;
}

eeskorka::HTTPContext::HTTPContext(int sd, serverConfig &cfg,
                                   loopCallbackType &cb)
        : sd(sd),
          logger(ServerLogger::get()),
          config(cfg),
          loopCallback(cb),
          unfinishedTask(false) {
    buffer = new char[config.bufferSize];

}

int eeskorka::HTTPContext::writeFile(const std::filesystem::path &p) {
    if (!unfinishedTask) {
        this->p = p;
        sizeLeft = fs::file_size(p);
        fReader.open(p);
    }

    int err = 0;
    while (sizeLeft != 0) {
        if (!unfinishedTask) {
            fReader.read(buffer, config.bufferSize);
        }

        unfinishedTask = false;
        if (sizeLeft >= config.bufferSize) {
            err = writeCompletely(buffer, config.bufferSize);
            if (err == -2) {
                unfinishedTask = true;
                return -2;
            } else if (err > 0) {
                sizeLeft -= config.bufferSize;
            }
        } else {
            err = writeCompletely(buffer, sizeLeft);
            if (err == -2) {
                unfinishedTask = true;
                return -2;
            } else if (err > 0) {
                sizeLeft = 0;
            }
        }

        if (err == -1) {
            logger.log(critical, "writecompletely fail, errno {}", strerror(errno));
            return -1;
        }
    }

    return 0;
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

bool eeskorka::HTTPContext::hasUnfinishedTask() {
    return unfinishedTask;
}

eeskorka::HTTPContext::~HTTPContext() {
    delete[] buffer;
}

eeskorka::HTTPContext::HTTPContext(eeskorka::HTTPContext &&c) noexcept : sd(c.sd), loopCallback(c.loopCallback),
                                                                         logger(c.logger),
                                                                         request(std::move(c.request)),
                                                                         response(std::move(c.response)),
                                                                         config(c.config) {}

eeskorka::HTTPContext::HTTPContext() : config(serverConfig()), logger(ServerLogger::get()) {

/*
 * TODO shared ptrs
 */
}
