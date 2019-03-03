//
// Created by silvman on 3/2/19.
//

#include "StaticHandler.h"


void eeskorka::handleStatic(eeskorka::HTTPContext &ctx) {
    if (ctx.request.requestLine.method == "GET" || ctx.request.requestLine.method == "HEAD") {
        std::string decodedURL;
        auto pos = ctx.request.requestLine.request_uri.find('?');
        if (pos == std::string::npos) {
            decodedURL = utility::URLDecode(ctx.request.requestLine.request_uri);
        } else {
            decodedURL = utility::URLDecode(ctx.request.requestLine.request_uri.substr(0, pos));
        }

        fs::path p;
        bool subdir = false;
        if (decodedURL[decodedURL.length() - 1] != '/') {
            if (fs::is_directory(ctx.config.rootDir + decodedURL)) {
                ctx.response.statusLine.reason_phrase = "Moved Permanently";
                ctx.response.headers["Location"] = utility::URLEncode(decodedURL + '/');
                ctx.response.statusLine.status_code = 301;

                ctx.writeHeader();
                ctx.close();
                return;
            }

            // no subdir
            p = ctx.config.rootDir + decodedURL;
        } else {
            // subdir
            subdir = true;
            p = ctx.config.rootDir + decodedURL + "index.html";
        }

        if (p.string().find("/..") == std::string::npos) {
            if (fs::exists(p)) {
                ServerLogger::get().log(info, "File exists");

                auto sizeLeft = fs::file_size(p);
                ctx.response.headers["Content-Length"] = fmt::format("{}", sizeLeft);
                ctx.response.headers["Content-Type"] = utility::getContentType(p);
                ctx.response.statusLine.reason_phrase = "OK";
                ctx.response.statusLine.status_code = 200;

                ctx.writeHeader();

                if (ctx.request.requestLine.method != "HEAD") {
                    ctx.writeFile(p);
                }
            } else if (subdir) {
                ServerLogger::get().log(info, "403");
                ctx.response.statusLine.reason_phrase = "Forbidden";
                ctx.response.statusLine.status_code = 403;

                ctx.writeHeader();
            } else {
                ServerLogger::get().log(info, "File not exists");
                ctx.response.statusLine.reason_phrase = "Not Found";
                ctx.response.statusLine.status_code = 404;

                ctx.writeHeader();
            }
        } else {
            ServerLogger::get().log(info, "400");
            ctx.response.statusLine.reason_phrase = "Bad Request";
            ctx.response.statusLine.status_code = 400;

            ctx.writeHeader();
        }
    } else {
        ctx.response.statusLine.reason_phrase = "Method Not Allowed";
        ctx.response.statusLine.status_code = 405;

        ctx.writeHeader();
    }

    ctx.close();
}
