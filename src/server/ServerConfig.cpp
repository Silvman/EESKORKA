//
// Created by silvman on 3/1/19.
//

#include "ServerConfig.h"
#include "../http/HTTPUtility.h"

int eeskorka::serverConfig::readConfigFile(const std::filesystem::path &p) {
    try {
        *this = eeskorka::utility::readConfig(p);
    } catch (const std::exception &e) {
        eeskorka::log(critical, e.what());
        return 1;
    }

    return 0;
}
