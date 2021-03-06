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
        void operator()(logLevel l, T str, const Args &... args) {
#ifndef NOLOG
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
#endif
        }

        static ServerLogger &get() {
            static ServerLogger S;
            return S;
        }

        ServerLogger(ServerLogger const &) = delete;
        void operator=(ServerLogger const &) = delete;

    private:
        ServerLogger() {
#ifndef NOLOG
            auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            spdlog::init_thread_pool(8192, 1);
            console = std::make_shared<spdlog::async_logger>("as", sink, spdlog::thread_pool(), spdlog::async_overflow_policy::block);

            console->set_pattern("[%H:%M:%f %z] [%^%l%$] [thread %t] %v");
            console->set_level(spdlog::level::debug);
#endif
        }

#ifndef NOLOG
        std::shared_ptr<spdlog::logger> console;
#endif

    };

    static ServerLogger& log = ServerLogger::get();
}


#endif //EESKORKA_LOGGER_H
