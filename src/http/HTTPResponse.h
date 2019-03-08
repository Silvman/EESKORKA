//
// Created by silvman on 3/1/19.
//

#ifndef EESKORKA_HTTPRESPONSE_H
#define EESKORKA_HTTPRESPONSE_H

#include <string>
#include <unordered_map>
#include <ctime>
#include <chrono>
#include "HTTPUtility.h"

namespace eeskorka {
    struct StatusLine {
        std::string http_version;
        int status_code {200};
        std::string reason_phrase;
    };

    struct HTTPResponse {
        HTTPResponse() {
            auto curtime = std::chrono::system_clock::now();
            headers["Date"] = utility::RFC1123Time(std::chrono::system_clock::to_time_t(curtime));

            headers["Connection"] = "close";
            headers["Content-Length"] = "0";
            headers["Server"] = "EESKORKA 0.1.1";
        }

        StatusLine statusLine;
        std::unordered_map<std::string, std::string> headers;

        std::string getHeader();
    };
}

#endif //EESKORKA_HTTPRESPONSE_H
