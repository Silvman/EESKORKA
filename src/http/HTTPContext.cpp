//
// Created by silvman on 3/2/19.
//

#include "HTTPContext.h"
#include <sys/sendfile.h>

eeskorka::IOStatus eeskorka::HTTPContext::writeCompletely(const char *buffer, size_t size) {
    while (written != size) {
        nn = write(sd, buffer + written, size - written);
        if (nn == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                return IOStatus::wouldBlock;
            }

            return IOStatus::error;
        }

        written += nn;
    }

    written = 0;
    return IOStatus::success;
}

eeskorka::HTTPContext::HTTPContext(int sd) : sd(sd), fd(0) {
    buffer = new char[config.bufferSize];
}

void eeskorka::HTTPContext::writeFile() {
    if (p.empty() || bodyDone)
        return;

    if (unfinishedTask == TaskType::no) {
        sizeLeft = fs::file_size(p);
        fd = open(p.c_str(), O_RDONLY);
        if (fd < 0) {
            log(critical, "can't open file, {}", strerror(errno));
            return;
        }
    }

    while (sizeLeft != 0) {
        unfinishedTask = TaskType::no; // resuming task
        written = sendfile(sd, fd, nullptr, sizeLeft);
        if (written != -1) {
            sizeLeft -= written;
        } else {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                unfinishedTask = TaskType::writeFile;
            } else {
                log(err, "sendfile, errono {}", strerror(errno));
            }

            return;
        }
    }

    bodyDone = true;
}

void eeskorka::HTTPContext::writeHeader() {
    if (headerDone) {
        return;
    }

    std::string responseHeader = response.getHeader();
    writeCompletely(responseHeader.c_str(), responseHeader.length());
    headerDone = true;
}

bool eeskorka::HTTPContext::hasUnfinishedTask() {
    return unfinishedTask != TaskType::no;
}

eeskorka::HTTPContext::~HTTPContext() {
    if (fd > 0) {
        close(fd);
    }

    delete[] buffer;
}

void eeskorka::HTTPContext::resumeTask() {
    if (unfinishedTask == TaskType::writeFile) {
        writeFile();
    }
}

void eeskorka::HTTPContext::attachFile(const std::filesystem::path &p) {
    this->p = p;
}
