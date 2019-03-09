//
// Created by silvman on 3/2/19.
//

#ifndef EESKORKA_STATICHANDLER_H
#define EESKORKA_STATICHANDLER_H

#include "../http/HTTPContext.h"

namespace eeskorka {
    void handleStatic(HTTPContext& ctx);
    void handleStaticRedirect(eeskorka::HTTPContext &ctx, const std::string& decodedURL);
    void handleStaticExists(eeskorka::HTTPContext &ctx, const fs::path& p);
}

#endif //EESKORKA_STATICHANDLER_H
