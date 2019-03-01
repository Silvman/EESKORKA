//
// Created by silvman on 3/2/19.
//

#ifndef EESKORKA_HTTPUTILITY_H
#define EESKORKA_HTTPUTILITY_H

#include <string>

namespace eeskorka {

    namespace utility {
        std::string URLDecode(const std::string &uri);

        std::string RFC1123Time(time_t time);

    }


}

#endif //EESKORKA_HTTPUTILITY_H
