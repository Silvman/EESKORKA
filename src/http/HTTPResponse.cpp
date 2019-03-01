//
// Created by silvman on 3/1/19.
//

#include "HTTPResponse.h"
#include <spdlog/fmt/fmt.h>

std::string eeskorka::HTTPResponse::getHeader() {
    std::string header;

    header += fmt::format("{} {} {}\r\n", statusLine.http_version, statusLine.status_code, statusLine.reason_phrase);
    std::for_each(headers.begin(), headers.end(),
                  [&header](auto pair) { header += fmt::format("{}: {}\r\n", pair.first, pair.second); });

    header += "\r\n";

    return header;
}
