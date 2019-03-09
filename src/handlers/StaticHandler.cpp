//
// Created by silvman on 3/2/19.
//

#include "StaticHandler.h"

void eeskorka::handleStaticRedirect(eeskorka::HTTPContext &ctx, const std::string &decodedURL) {
    ctx.response.headers["Location"] = utility::URLEncode(decodedURL + '/');
    ctx.response.setStatusCode(StatusCode::MovedPermanently);
}

void eeskorka::handleStaticExists(eeskorka::HTTPContext &ctx, const fs::path &p) {
    auto sizeLeft = fs::file_size(p);
    ctx.response.headers["Content-Length"] = fmt::format("{}", sizeLeft);
    ctx.response.setContentTypeByExtension(p.extension());
    ctx.response.setStatusCode(StatusCode::OK);

    if (ctx.request.requestLine.method != "HEAD")
        ctx.attachFile(p);
}

void eeskorka::handleStatic(eeskorka::HTTPContext &ctx) {
    if (ctx.request.requestLine.method != "GET" && ctx.request.requestLine.method != "HEAD") {
        ctx.response.setStatusCode(StatusCode::MethodNotAllowed);
        return;
    }

    if (ctx.request.requestLine.request_uri.find("/..") != std::string::npos) {
        ctx.response.setStatusCode(StatusCode::BadRequest);
        return;
    }

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
        if (fs::is_directory(eeskorka::config.rootDir + decodedURL)) {
            handleStaticRedirect(ctx, decodedURL);
            return;
        }

        // no subdir
        p = eeskorka::config.rootDir + decodedURL;
    } else {
        // subdir
        subdir = true;
        p = eeskorka::config.rootDir + decodedURL + "index.html";
    }

    if (fs::exists(p)) {
        handleStaticExists(ctx, p);
    } else if (subdir) {
        ctx.response.setStatusCode(StatusCode::Forbidden);
    } else {
        ctx.response.setStatusCode(StatusCode::NotFound);
    }
}
