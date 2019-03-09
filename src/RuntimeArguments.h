//
// Created by silvman on 3/4/19.
//

#ifndef EESKORKA_RUNTIMEARGUMENTS_H
#define EESKORKA_RUNTIMEARGUMENTS_H


#include <string>

struct RuntimeArguments {
    int port {-1};
    std::string configPath {"/etc/httpd.conf"};

    RuntimeArguments(int argc, char** argv);
};


#endif //EESKORKA_RUNTIMEARGUMENTS_H
