#include "server/HTTPServer.h"
#include "logger/ServerLogger.h"

typedef eeskorka::ServerLogger loge;

int main() {
    loge& log = eeskorka::ServerLogger::get();

    // todo config reading
    eeskorka::serverConfig config;

    eeskorka::httpServer server(config);
    if (server.startStaticServer() == -1) {
        log.log(eeskorka::critical, "cannot start server");
    }

    return 0;
}
