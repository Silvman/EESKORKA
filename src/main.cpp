#include "server/HTTPServer.h"
#include "logger/ServerLogger.h"
#include "http/HTTPUtility.h"

int main() {
    eeskorka::serverConfig config("../config.conf");
    eeskorka::httpServer server(config);
    if (server.startStaticServer() == -1) {
        spdlog::critical("cannot start server");
        return 1;
    }

    return 0;
}
