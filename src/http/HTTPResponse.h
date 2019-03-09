//
// Created by silvman on 3/1/19.
//

#ifndef EESKORKA_HTTPRESPONSE_H
#define EESKORKA_HTTPRESPONSE_H

#include <string>
#include <unordered_map>
#include <ctime>
#include <chrono>
#include "HTTPUtility.h"

namespace eeskorka {
    constexpr const char *Close = "Close";
    constexpr const char *KeepAlive = "Keep-Alive";
    constexpr const char *ServerName = "EESKORKA 0.2.1";

    enum class StatusCode {
        Continue = 100,
        SwitchingProtocols = 101,
        Processing = 102,
        EarlyHints = 103,

        OK = 200,
        Created = 201,
        Accepted = 202,
        NonAuthoritativeInformation = 203,
        NoContent = 204,
        ResetContent = 205,
        PartialContent = 206,
        MultiStatus = 207,
        AlreadyReported = 208,
        IMUsed = 226,

        MultipleChoices = 300,
        MovedPermanently = 301,
        Found = 302,
        SeeOther = 303,
        NotModified = 304,
        UseProxy = 305,

        TemporaryRedirect = 307,
        PermanentRedirect = 308,

        BadRequest = 400,
        Unauthorized = 401,
        PaymentRequired = 402,
        Forbidden = 403,
        NotFound = 404,
        MethodNotAllowed = 405,
        NotAcceptable = 406,
        ProxyAuthenticationRequired = 407,
        RequestTimeout = 408,
        Conflict = 409,
        Gone = 410,
        LengthRequired = 411,
        PreconditionFailed = 412,
        PayloadTooLarge = 413,
        URITooLong = 414,
        UnsupportedMediaType = 415,
        RangeNotSatisfiable = 416,
        ExpectationFailed = 417,
        ImATeapot = 418,
        UnprocessableEntity = 422,
        Locked = 423,
        FailedDependency = 424,
        UpgradeRequired = 426,
        PreconditionRequired = 428,
        TooManyRequests = 429,
        RequestHeaderFieldsTooLarge = 431,
        UnavailableForLegalReasons = 451,

        InternalServerError = 500,
        NotImplemented = 501,
        BadGateway = 502,
        ServiceUnavailable = 503,
        GatewayTimeout = 504,
        HTTPVersionNotSupported = 505,
        VariantAlsoNegotiates = 506,
        InsufficientStorage = 507,
        LoopDetected = 508,
        NotExtended = 510,
        NetworkAuthenticationRequired = 511,
    };

    struct StatusLine {
        std::string http_version;
        int status_code {200};
    };

    enum class ConnectionType {
        close = 0,
        keepAlive,
    };

    class HTTPResponse {
    public:
        HTTPResponse() {
            headers.reserve(8);
            auto curtime = std::chrono::system_clock::now();
            headers["Date"] = utility::RFC1123Time(std::chrono::system_clock::to_time_t(curtime));
            headers["Connection"] = Close;
            headers["Content-Length"] = "0";
            headers["Server"] = ServerName;
        }

        void setConnectionType(ConnectionType ct) {
            switch (ct) {
                case ConnectionType::keepAlive: {
                    headers["Connection"] = KeepAlive;
                    break;
                }

                case ConnectionType::close: {
                    headers["Connection"] = Close;
                }
            }
        }

        void setContentLength(size_t len) {
            headers["Content-Length"] = fmt::format("{}", len);
        }

        void setStatusCode(StatusCode code) {
            statusLine.status_code = static_cast<int>(code);
        };

        void setContentTypeByExtension(const std::string &ext) {
            static std::unordered_map<std::string, std::string> mime = {
                    {".html", "text/html"},
                    {".css",  "text/css"},
                    {".js",   "text/javascript"},
                    {".jpg",  "image/jpeg"},
                    {".jpeg", "image/jpeg"},
                    {".png",  "image/png"},
                    {".gif",  "image/gif"},
                    {".swf",  "application/x-shockwave-flash"}
            };

            if (mime.find(ext) != mime.end()) {
                headers["Content-Type"] = mime[ext];
            } else {
                headers["Content-Type"] = "text/plain";
            }
        }

        std::string getHeader();

        std::unordered_map<std::string, std::string> headers;
        StatusLine statusLine;

    private:
        inline std::string getReasonPhrase(int code) {
            switch (code) {
                case 100: return "Continue";
                case 101: return "Switching Protocols";
                case 102: return "Processing";
                case 103: return "Early Hints";

                case 200: return "OK";
                case 201: return "Created";
                case 202: return "Accepted";
                case 203: return "Non-Authoritative Information";
                case 204: return "No Content";
                case 205: return "Reset Content";
                case 206: return "Partial Content";
                case 207: return "Multi-Status";
                case 208: return "Already Reported";
                case 226: return "IM Used";

                case 300: return "Multiple Choices";
                case 301: return "Moved Permanently";
                case 302: return "Found";
                case 303: return "See Other";
                case 304: return "Not Modified";
                case 305: return "Use Proxy";
                case 307: return "Temporary Redirect";
                case 308: return "Permanent Redirect";

                case 400: return "Bad Request";
                case 401: return "Unauthorized";
                case 402: return "Payment Required";
                case 403: return "Forbidden";
                case 404: return "Not Found";
                case 405: return "Method Not Allowed";
                case 406: return "Not Acceptable";
                case 407: return "Proxy Authentication Required";
                case 408: return "Request Timeout";
                case 409: return "Conflict";
                case 410: return "Gone";
                case 411: return "Length Required";
                case 412: return "Precondition Failed";
                case 413: return "Payload Too Large";
                case 414: return "URI Too Long";
                case 415: return "Unsupported Media Type";
                case 416: return "Range Not Satisfiable";
                case 417: return "Expectation Failed";
                case 418: return "I'm a teapot";
                case 422: return "Unprocessable Entity";
                case 423: return "Locked";
                case 424: return "Failed Dependency";
                case 426: return "Upgrade Required";
                case 428: return "Precondition Required";
                case 429: return "Too Many Requests";
                case 431: return "Request Header Fields Too Large";
                case 451: return "Unavailable For Legal Reasons";

                case 500: return "Internal Server Error";
                case 501: return "Not Implemented";
                case 502: return "Bad Gateway";
                case 503: return "Service Unavailable";
                case 504: return "Gateway Time-out";
                case 505: return "HTTP Version Not Supported";
                case 506: return "Variant Also Negotiates";
                case 507: return "Insufficient Storage";
                case 508: return "Loop Detected";
                case 510: return "Not Extended";
                case 511: return "Network Authentication Required";

                default: return std::string();
            }
        }
    };
}

#endif //EESKORKA_HTTPRESPONSE_H
