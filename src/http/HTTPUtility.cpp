//
// Created by silvman on 3/2/19.
//

#include <string>
#include <unordered_map>
#include <iomanip>
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

eeskorka::serverConfig eeskorka::utility::readConfig(const std::filesystem::path &path) {
    if (!fs::exists(path)) {
        throw std::runtime_error(fmt::format("file not exists: {}", path.string()));
    }

    serverConfig config;

    std::ifstream f(path);
    std::string buf;
    std::regex re("([^ \t\r\n\v\f]+[A-Za-z0-9!\"#$%&'()*+,./:;<=>?@\\^_`{|}~-]*)");

    while (!f.eof()) {
        std::getline(f, buf);

        auto commentIt = std::find(buf.begin(), buf.end(), '#');

        for (std::sregex_token_iterator i{buf.begin(), commentIt, re, 0}, end; i != end; ++i) {
            std::string token{*i};

            if (token == "cpu_limit") {
                ++i;
                config.workers = std::atoi((*i).str().c_str());

                if (config.workers < 1) {
                    throw std::runtime_error(
                            fmt::format("config parse failed: cpu_limit == {} (must be at least 1)", config.workers));
                }

                break;
            }

            if (token == "thread_limit") {
                ++i;
                config.numThreads = std::atoi((*i).str().c_str());

                if (config.workers < 1) {
                    throw std::runtime_error(
                            fmt::format("config parse failed: thread_limit == {} (must be at least 1)",
                                        config.workers));
                }

                break;
            }

            if (token == "document_root") {
                ++i;
                config.rootDir = (*i).str();

                if (!fs::exists(config.rootDir)) {
                    throw std::runtime_error(
                            fmt::format("config parse failed: {} does not exists", config.rootDir));
                }

                if (!fs::is_directory(config.rootDir)) {
                    throw std::runtime_error(
                            fmt::format("config parse failed: {} is not a directory", config.rootDir));
                }

                break;
            }

            if (token == "buffer_size") {
                ++i;
                config.bufferSize = static_cast<size_t>(std::atoi((*i).str().c_str()));

                if (config.bufferSize < 32) {
                    throw std::runtime_error(
                            fmt::format("config parse failed: buffer_size must be greater than 32"));
                }
            }

            if (token == "port") {
                ++i;
                config.port = std::atoi((*i).str().c_str());

                if (config.port < 0 || config.port >= 65536) {
                    throw std::runtime_error(
                            fmt::format("config parse failed: port must be 0 <= {} < 65536", config.port));
                }
            }

            if (token == "max_clients") {
                ++i;
                config.maxClients = std::atoi((*i).str().c_str());

                if (config.maxClients < config.workers) {
                    throw std::runtime_error(
                            fmt::format(
                                    "config parse failed: max_clients ({}) must be greather "
                                    "or equeal to max_cores ({})",
                                    config.maxClients, config.workers));
                }
            }
        }
    }

    return config;
}

std::string eeskorka::utility::URLEncode(const std::string &uri) {
    std::string enc;
    char bufHex[10];

    for (const auto &rune : uri) {
        if (rune == ' ') {
            enc += '+';
        } else if (isalnum(rune) || rune == '-' || rune == '_' || rune == '.' || rune == '~' || rune == '/')  {
            enc += rune;
        } else {
            enc += fmt::format("%{:02X}", rune);
        }
    }

    return enc;
}
