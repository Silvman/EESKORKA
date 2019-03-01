//
// Created by silvman on 3/1/19.
//

#include "httpServer.h"

std::string eeskorka::toLowerCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

void eeskorka::HTTPMessage::parseRawHeader(const std::string &raw) {
    std::istringstream rawReader(raw);
    std::string header;

    // RequestLine parsing
    std::getline(rawReader, header);
    auto spIndex1 = header.find(' ');
    auto spIndex2 = header.find(' ', spIndex1 + 1);
    requestLine.method = header.substr(0, spIndex1);
    requestLine.request_uri = header.substr(spIndex1 + 1, spIndex2 - spIndex1 - 1);
    requestLine.http_version = header.substr(spIndex2 + 1, header.length() - spIndex2 - 2);

    // Headers parsing
    while (std::getline(rawReader, header) && header != "\r") {
        auto sepIndex = header.find(':');
        if (sepIndex != std::string::npos) {
            headers.insert(std::make_pair(
                    toLowerCase(header.substr(0, sepIndex)),
                    header.substr(sepIndex + 2, header.length() - sepIndex - 3) // todo idk if SP is RFC required
            ));
        }
    }

#ifdef DEBUG
    spdlog::debug("HTTP headers parsing done");
        spdlog::debug("Method: {}, "
                      "Request-URI: {}, "
                      "HTTP-Version: {}",
                      requestLine.method,
                      requestLine.request_uri,
                      requestLine.http_version);

        std::for_each(headers.begin(), headers.end(),
                      [](auto pair) { spdlog::debug("{}: {}", pair.first, pair.second); });
#endif

}

bool eeskorka::httpServer::isHeaderOver(const std::string &s) {
    return s.find("\r\n\r\n") != std::string::npos;
}

int eeskorka::httpServer::writeCompletely(int fd, const char *buffer, size_t size) {
    ssize_t written = 0, nn = 0;

    while (written != size) {
        nn = write(fd, buffer, size);
        if (nn == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                spdlog::debug("writeCompletely: EAGAIN");
                return 0;
            }

            return -1;
        }

        written += nn;
    }

    return 0;
}

int eeskorka::httpServer::startStaticServer() {
    basicServer.setClientCallback(std::bind(&httpServer::onClientReady, this, std::placeholders::_1));
    int err = basicServer.start();
    return err;
}

int eeskorka::httpServer::onClientReady(int sd) {
    HTTPMessage httpMessage;
    std::string rawMessage;

    char buffer[config.bufferSize];

    loop {
        ssize_t n = read(sd, buffer, config.bufferSize);
        spdlog::debug("n: {}", n);

        if (n > 0) {
            rawMessage.append(buffer, n);
        } else {
            if (n == -1) {
                // прочитали сообщение
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    spdlog::debug("read message, errno eagain");
                    break;
                } else {
                    spdlog::critical("read message, errno: {}", strerror(errno));
                    return -1;
                }
            } else {
                spdlog::debug("n=0, disconnect");
                return 0;
            }
        }
    };

    if (isHeaderOver(rawMessage)) {
        spdlog::info("header is read");
    } else {
        spdlog::warn("header is broken");
        return 0;
    }

    httpMessage.parseRawHeader(rawMessage);
    std::string responseHeader;

    fs::path p = "." + httpMessage.requestLine.request_uri;

    if (fs::exists(p)) {
        spdlog::debug("file exists");

        auto sizeLeft = fs::file_size(p);

        responseHeader += fmt::format("{} 200 OK\r\n", httpMessage.requestLine.http_version);
        responseHeader += fmt::format("Content-Length: {}\r\n", sizeLeft);
        responseHeader += "\r\n";

        if (writeCompletely(sd, responseHeader.c_str(), responseHeader.length()) == -1) {
            spdlog::debug("writecompletely fail");
            return 0;
        }

        std::ifstream fReader;
        fReader.open(p);
        while (sizeLeft != 0) {
            fReader.read(buffer, config.bufferSize);
            int err = 0;

            if (sizeLeft >= config.bufferSize) {
                err = writeCompletely(sd, buffer, config.bufferSize);
                sizeLeft -= config.bufferSize;
            } else {
                err = writeCompletely(sd, buffer, sizeLeft);
                sizeLeft = 0;
            }

            if (err != 0) {
                spdlog::critical("writecompletely fail");
                break;
            }
        }
    } else {
        spdlog::debug("file not exists");
        responseHeader += fmt::format("{} 404 Not Found\r\n", httpMessage.requestLine.http_version);
        responseHeader += fmt::format("Content-Length: 0\r\n");
        responseHeader += fmt::format("Connection: close\r\n\r\n");

        writeCompletely(sd, responseHeader.c_str(), responseHeader.length());
    }

    return 0;
}
