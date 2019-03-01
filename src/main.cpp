#include "spdlog/spdlog.h"
#include "server/httpServer.h"

int main() {
    spdlog::set_pattern("[%H:%M:%S %z] [%^%l%$] [thread %t] %v");
    spdlog::set_level(spdlog::level::debug);

    // config reading
    eeskorka::serverConfig config;

    eeskorka::httpServer server(config);
    if (server.startStaticServer() == -1) {
        spdlog::critical("cannot start server");
    }

    return 0;
}
