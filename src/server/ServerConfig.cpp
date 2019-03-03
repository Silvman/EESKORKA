//
// Created by silvman on 3/1/19.
//

#include "ServerConfig.h"
#include "../http/HTTPUtility.h"

eeskorka::serverConfig::serverConfig(const std::filesystem::path &p) {
    try {
        *this = eeskorka::utility::readConfig(p);
    } catch (const std::exception &e) {
        ServerLogger::get().log(critical, e.what());
    }
}
