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

    enum class IOStatus {
        wouldBlock = -2,
        error = -1,
        success = 0,
    };

    enum class TaskType {
        no = 0,
        writeFile = 1,
        writeData,
        read,
    };

    class HTTPContext {
    public:
        explicit HTTPContext(int sd = -1);
        virtual ~HTTPContext();

        HTTPRequest request;
        HTTPResponse response;

        void writeHeader();
        void attachFile(const fs::path &p);

        void writeFile();

        bool hasUnfinishedTask();
        void resumeTask();
        int sd;

    private:
        TaskType unfinishedTask {TaskType::no};
        IOStatus writeCompletely(const char *buffer, size_t size);

        ssize_t written {0}, nn {0};
        uintmax_t sizeLeft {0};

        bool headerDone {false};
        bool bodyDone {false};

        std::filesystem::path p;

        char *buffer;
        int fd;
    };
}


#endif //EESKORKA_HTTPCONTEXT_H
