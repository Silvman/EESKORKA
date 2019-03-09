#include "server/HTTPServer.h"
#include "RuntimeArguments.h"

eeskorka::serverConfig eeskorka::config;

int main(int argc, char** argv) {
    RuntimeArguments arguments(argc, argv);

    eeskorka::config.readConfigFile(arguments.configPath);
    if (arguments.port != -1) {
        eeskorka::config.port = arguments.port;
    }

    eeskorka::httpServer server;
    if (server.startStaticServer() == -1) {
        spdlog::critical("cannot start server");
        return 1;
    }

    return 0;
}
