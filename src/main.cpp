#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <thread>
#include <csignal>
#include <sstream>
#include "spdlog/spdlog.h"
#include <filesystem>
#include <fstream>

#define loop for (;;)

#define DEBUG

#define NUM_THREADS 4
const int MAX_CLIENTS = 100;
const int BUF_SIZE = 128;

const char EPOLL_CREATE_ERR[] = "Failed to create epoll file descriptor\n";
const char EPOLL_CLOSE_ERR[] = "Failed to close epoll file descriptor\n";
const char SOCK_CREATE_ERR[] = "Failed to create socket descriptor\n";
const char SOCK_CLOSE_ERR[] = "Failed to close socket descriptor\n";

namespace fs = std::filesystem;

/**
 * делать несколько еполлов нельзя - будет вешаться на подключении новых клиентов при блокирующем сокете
 * придется делать мутех на евентаъ...
 * */

/*
 * info
 * error
 * warn
 * critical
 * debug
 */

std::string IPAddressToString(int ip) {
    return fmt::format("{}.{}.{}.{}",
                       (ip >> 24) & 0xFF,
                       (ip >> 16) & 0xFF,
                       (ip >> 8) & 0xFF,
                       (ip) & 0xFF);
}

bool isHeaderOver(const std::string &s) {
    return s.find("\r\n\r\n") != std::string::npos;
}

struct RequestLine {
    std::string method;
    std::string request_uri;
    std::string http_version;
};

// idk wat im doin
std::string toLowerCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

std::mutex events_mtx;

struct HTTPMessage {
    RequestLine requestLine;
    std::unordered_map<std::string, std::string> headers;
//    std::string body;

    void parseRawHeader(const std::string &raw) {
        std::istringstream rawReader(raw);
        std::string header;

        // RequestLine parsing
        std::getline(rawReader, header);
        auto spIndex1 = header.find(' ');
        auto spIndex2 = header.find(' ', spIndex1 + 1);
        requestLine.method = header.substr(0, spIndex1);
        requestLine.request_uri = header.substr(spIndex1 + 1, spIndex2 - spIndex1 - 1);
        requestLine.http_version = header.substr(spIndex2 + 1, header.length() - spIndex2 - 2);

        // Headers parsing
        while (std::getline(rawReader, header) && header != "\r") {
            auto sepIndex = header.find(':');
            if (sepIndex != std::string::npos) {
                headers.insert(std::make_pair(
                        toLowerCase(header.substr(0, sepIndex)),
                        header.substr(sepIndex + 2, header.length() - sepIndex - 3) // todo idk if SP is RFC required
                ));
            }
        }

#ifdef DEBUG
        spdlog::debug("HTTP headers parsing done");
        spdlog::debug("Method: {}, "
                      "Request-URI: {}, "
                      "HTTP-Version: {}",
                      requestLine.method,
                      requestLine.request_uri,
                      requestLine.http_version);

        std::for_each(headers.begin(), headers.end(),
                      [](auto pair) { spdlog::debug("{}: {}", pair.first, pair.second); });
#endif

    }
};


void workloop(int sd) {
    int epfd = epoll_create(MAX_CLIENTS);
    if (epfd == -1) {
        spdlog::critical(EPOLL_CREATE_ERR);
        return;
    }
    spdlog::info("create epoll, epfd {}", epfd);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sd;

    int err = epoll_ctl(epfd, EPOLL_CTL_ADD, sd, &ev);
    if (err != 0) {
        spdlog::critical("epoll ctl failed");
        return;
    }

    struct epoll_event *events = (struct epoll_event *) malloc(MAX_CLIENTS * sizeof(struct epoll_event));

    spdlog::info("bind epoll to the socket");

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    
    spdlog::info("EESKORKA server is better than nginx >_> {}", "yay" );

    loop {
        int epoll_ret = epoll_wait(epfd, events, MAX_CLIENTS, -1);

        for (int i = 0; i < epoll_ret; i++) {
            if (events[i].data.fd == sd) {
                if (events_mtx.try_lock()) { // best is some kind of cond varaible
                    spdlog::debug("yeah baby");

                    struct sockaddr_in client;
                    socklen_t cli_len = sizeof(client);
                    int cli_sd = accept(sd, (struct sockaddr *) &client, &cli_len);

                    setsockopt(cli_sd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));

                    struct epoll_event cli_ev;
                    cli_ev.events = EPOLLIN | EPOLLET; // переключить в режим на чтение! в http сначала запрос
                    // cli_ev.data.ptr = nullptr;
                    cli_ev.data.fd = cli_sd;

                    epoll_ctl(epfd, EPOLL_CTL_ADD, cli_sd, &cli_ev);
                    // todo add check

                    spdlog::info("new client: sd {}, from {}", cli_sd, IPAddressToString(client.sin_addr.s_addr));

                    events_mtx.unlock();
                }

            } else if (events[i].events & EPOLLIN) {
                HTTPMessage httpMessage;
                std::string rawMessage;
                char buf[BUF_SIZE];

                loop {
                    auto n = recv(events[i].data.fd, buf, BUF_SIZE, 0);
                    spdlog::debug("n: {}", n);

                    if (n == -1) {
                        // what is EAGAIN about?
                        epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
                        close(events[i].data.fd);
                        break;
                    }

                    if (n == 0) {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
                        close(events[i].data.fd);
                        break;
                    }

                    rawMessage.append(buf, n);

                    if (isHeaderOver(rawMessage)) {
                        spdlog::info("header is read");
                        httpMessage.parseRawHeader(rawMessage);
                        std::string responseHeader;

                        fs::path p = "." + httpMessage.requestLine.request_uri;
                        if (fs::exists(p)) {
                            spdlog::debug("file exists");

                            auto sizeLeft = fs::file_size(p);

                            responseHeader += fmt::format("{} 200 OK\r\n", httpMessage.requestLine.http_version);
                            responseHeader += fmt::format("Content-Length: {}\r\n", sizeLeft);
                            responseHeader += "\r\n";

                            send(events[i].data.fd, responseHeader.c_str(), responseHeader.length(), 0);

                            std::ifstream fReader;
                            fReader.open(p);
                            loop {
                                fReader.read(buf, BUF_SIZE);
                                if (sizeLeft >= BUF_SIZE) {
                                    int err = send(events[i].data.fd, buf, BUF_SIZE, 0);
                                    if (err == -1) {
                                        break; // todo sig broken pipe
                                    }
                                    sizeLeft -= BUF_SIZE;
                                } else {
                                    send(events[i].data.fd, buf, sizeLeft, 0);
                                    break;
                                }
                            };
                        } else {
                            spdlog::debug("file not exists");
                            responseHeader += fmt::format("{} 404 Not Found\r\n", httpMessage.requestLine.http_version);
                            responseHeader += fmt::format("Content-Length: 0\r\n");
                            responseHeader += fmt::format("Connection: close\r\n\r\n");


                            send(events[i].data.fd, responseHeader.c_str(), responseHeader.length(), 0);
                        }

                        if (httpMessage.headers["connection"] != "keep-alive") {
                            spdlog::debug("lololo http1.0 kek");

                            epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
                            close(events[i].data.fd);
                        }

                        break;
                    }
                };

                // todo logic happens here

                // todo send happens here
            } else if (events[i].events & EPOLLOUT) {
                spdlog::debug("hallo");
            }
        }
    }


    if (close(epfd) != 0) {
        fprintf(stderr, EPOLL_CLOSE_ERR);
        return;
    }
}

int main() {
    spdlog::set_pattern("[%H:%M:%S %z] [%^%l%$] [thread %t] %v");
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("server is starting");

    int err = 0;
    int sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd == -1) {
        spdlog::critical(SOCK_CREATE_ERR);
        return 1;
    }
    spdlog::info("open socket, sd {}", sd);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;

    setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));

    struct sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    err = bind(sd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (err != 0) {
        spdlog::critical("bind failed");
        return 1;
    }
    spdlog::info("bind port to the socket");

    err = listen(sd, MAX_CLIENTS);
    if (err != 0) {
        spdlog::critical("listen failed");
        return 1;
    }
    spdlog::info("start listening for new connections");

    std::vector<std::thread> threadfork;
    for (int i = 0; i < NUM_THREADS - 1; ++i) {
        threadfork.emplace_back(workloop, sd);
    }

    workloop(sd);

    for (auto &thread : threadfork) {
        thread.join();
    }

    if (close(sd) != 0) {
        fprintf(stderr, SOCK_CLOSE_ERR);
        return 1;
    }

    return 0;
}
