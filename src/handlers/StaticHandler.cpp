//
// Created by silvman on 3/2/19.
//

#include "StaticHandler.h"


void eeskorka::handleStatic(eeskorka::HTTPContext &ctx) {
    if (ctx.request.requestLine.method == "GET" || ctx.request.requestLine.method == "HEAD") {
        std::string encodedURI;
        auto pos = ctx.request.requestLine.request_uri.find('?');
        if (pos == std::string::npos) {
            encodedURI = utility::URLDecode(ctx.request.requestLine.request_uri);
        } else {
            encodedURI = utility::URLDecode(ctx.request.requestLine.request_uri.substr(0, pos));
        }

        fs::path p;
        bool subdir = false;
        if (encodedURI[encodedURI.length() - 1] != '/') {
            // no subdir
            p = ctx.config.rootDir + encodedURI;
        } else {
            // subdir
            subdir = true;
            p = ctx.config.rootDir + encodedURI + "index.html";
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
