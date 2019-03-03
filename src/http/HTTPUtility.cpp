//
// Created by silvman on 3/2/19.
//

#include <string>
#include <unordered_map>
#include "HTTPUtility.h"

std::string eeskorka::utility::URLDecode(const std::string &uri) {
    std::string ret;
    char rune = 0;
    unsigned coded = 0;
    size_t len = uri.length();

    for (unsigned i = 0; i < len; i++) {
        if (uri[i] != '%') {
            if (uri[i] == '+')
                ret += ' ';
            else
                ret += uri[i];
        } else {
            sscanf(uri.substr(i + 1, 2).c_str(), "%x", &coded);
            rune = static_cast<char>(coded);
            ret += rune;
            i = i + 2;
        }
    }

    return ret;
}

std::string eeskorka::utility::RFC1123Time(time_t time) {
    char buffer[80];

    struct tm *timeinfo;
    timeinfo = gmtime(&time);
    strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);

    return buffer;
}


std::string eeskorka::utility::getContentType(const fs::path &path) {
    static std::unordered_map<std::string, std::string> mime = {
            {".html", "text/html"},
            {".css",  "text/css"},
            {".js",   "text/javascript"},
            {".jpg",  "image/jpeg"},
            {".jpeg", "image/jpeg"},
            {".png",  "image/png"},
            {".gif",  "image/gif"},
            {".swf",  "application/x-shockwave-flash"}
    };

    std::string ext = path.extension();

    if (mime.find(ext) != mime.end()) {
        return mime[ext];
    }

    return "text/plain";
}