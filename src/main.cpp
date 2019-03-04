#include "server/HTTPServer.h"
#include "RuntimeArguments.h"

int main(int argc, char** argv) {
    RuntimeArguments arguments(argc, argv);

    eeskorka::serverConfig config(arguments.configPath);
    if (arguments.port != -1) {
        config.port = arguments.port;
    }

    eeskorka::httpServer server(config);
    if (server.startStaticServer() == -1) {
        spdlog::critical("cannot start server");
        return 1;
    }

    return 0;
}
