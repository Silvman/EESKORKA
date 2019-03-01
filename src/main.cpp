#include "server/httpServer.h"
#include "logger/serverLogger.h"

typedef eeskorka::serverLogger loge;

int main() {
    loge& log = eeskorka::serverLogger::get();

    // todo config reading
    eeskorka::serverConfig config;

    eeskorka::httpServer server(config);
    if (server.startStaticServer() == -1) {
        log.log(eeskorka::critical, "cannot start server");
    }

    return 0;
}
