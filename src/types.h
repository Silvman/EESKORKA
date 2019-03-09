//
// Created by silvman on 3/3/19.
//

#ifndef EESKORKA_TYPES_H
#define EESKORKA_TYPES_H

#include <functional>
#include "http/HTTPContext.h"

enum connectionAction {
    rearmConnection = 100,
    closeConnection,
    waitUntillReaded,
};

typedef std::function<void(eeskorka::HTTPContext*, connectionAction)> loopCallbackType;
typedef std::function<int(eeskorka::HTTPContext*, loopCallbackType &)> clientCallbackType;

#endif //EESKORKA_TYPES_H
