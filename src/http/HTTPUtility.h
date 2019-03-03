//
// Created by silvman on 3/2/19.
//

#ifndef EESKORKA_HTTPUTILITY_H
#define EESKORKA_HTTPUTILITY_H

#include <string>
#include <filesystem>

namespace eeskorka {
    namespace fs = std::filesystem;

    namespace utility {

        std::string URLDecode(const std::string &uri);

        std::string RFC1123Time(time_t time);

        std::string getContentType(const fs::path& path);

    }


}

#endif //EESKORKA_HTTPUTILITY_H
