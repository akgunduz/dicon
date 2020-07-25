//
// Created by Haluk Akgunduz on 10/17/15.
//

#include "StopWatch.h"

void StopWatch::start() {

    tStart = time(nullptr);

    initiated = true;
}

long StopWatch::stop() {

    if (!initiated) {
        return 0;
    }

    tStop = time(nullptr);

    return tStop - tStart;

}

void StopWatch::reset() {

    initiated = false;

}

bool StopWatch::isInitiated() {

    return initiated;

}