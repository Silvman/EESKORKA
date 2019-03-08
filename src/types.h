//
// Created by silvman on 3/3/19.
//

#ifndef EESKORKA_TYPES_H
#define EESKORKA_TYPES_H

#include <functional>

enum connectionAction {
    rearmConnection = 100,
    closeConnection,
    waitUntillReaded,
};

typedef std::function<void(int, connectionAction)> loopCallbackType;
typedef std::function<int(int, loopCallbackType &)> clientCallbackType;

#endif //EESKORKA_TYPES_H
