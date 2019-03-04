//
// Created by silvman on 3/4/19.
//

#include <spdlog/spdlog.h>
#include <filesystem>
#include "RuntimeArguments.h"

RuntimeArguments::RuntimeArguments(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--port") {
            if (i + 1 < argc) {
                std::string val = argv[i + 1];
                if (std::strtol(argv[i + 1], nullptr, 10) >= 0 && errno != EINVAL) {
                    port = static_cast<int>(std::strtol(argv[i + 1], nullptr, 10));
                    ++i;
                    continue;
                } else {
                    spdlog::critical("bad port value: {}", val);
                }
            } else {
                spdlog::critical("no port value");
            }
        }

        if (arg == "--config") {
            if (i + 1 < argc) {
                std::string val = argv[i + 1];
                if (std::filesystem::exists(val)) {
                    configPath = val;
                    ++i;
                    continue;
                } else {
                    spdlog::critical("not exists: {}", val);
                }
            } else {
                spdlog::critical("no config value");
            }
        }
    }
}
