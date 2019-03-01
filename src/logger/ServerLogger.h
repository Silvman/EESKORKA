//
// Created by silvman on 3/1/19.
//

#ifndef EESKORKA_LOGGER_H
#define EESKORKA_LOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/async.h" //support for async logging.
#include "spdlog/sinks/stdout_color_sinks.h"

namespace eeskorka {
    enum logLevel {
        info = 100,
        warn,
        err,
        critical,
        debug
    };

    class ServerLogger {
    public:
        template<typename... Args, typename T>
        void log(logLevel l, T str, const Args &... args) {
            switch (l) {
                case info: {
                    console->info(str, args...);
                    break;
                }

                case warn: {
                    console->warn(str, args...);
                    break;
                }

                case err: {
                    console->error(str, args...);
                    break;
                }

                case critical: {
                    console->critical(str, args...);
                    break;
                }

                case debug: {
                    console->debug(str, args...);
                    break;
                }
            }
        }

        static ServerLogger &get() {
            static ServerLogger S;
            return S;
        }

        ServerLogger(ServerLogger const &) = delete;
        void operator=(ServerLogger const &) = delete;

    private:
        ServerLogger() {
            auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            spdlog::init_thread_pool(8192, 1);
            console = std::make_shared<spdlog::async_logger>("as", sink, spdlog::thread_pool(), spdlog::async_overflow_policy::block);

            console->set_pattern("[%H:%M:%S %z] [%^%l%$] [thread %t] %v");
            console->set_level(spdlog::level::debug);
        }

        std::shared_ptr<spdlog::logger> console;
    };

}


#endif //EESKORKA_LOGGER_H
