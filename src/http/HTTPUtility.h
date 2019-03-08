//
// Created by silvman on 3/2/19.
//

#ifndef EESKORKA_HTTPUTILITY_H
#define EESKORKA_HTTPUTILITY_H

#include <string>
#include <filesystem>
#include <regex>
#include <fstream>
#include "../server/ServerConfig.h"
#include "../logger/ServerLogger.h"


namespace eeskorka {
    namespace fs = std::filesystem;

    namespace utility {
        std::string URLDecode(const std::string &uri);
        std::string URLEncode(const std::string &uri);

        std::string RFC1123Time(time_t time);
        std::string getContentType(const fs::path &path);

        serverConfig readConfig(const fs::path &path);

    }


}

#endif //EESKORKA_HTTPUTILITY_H
