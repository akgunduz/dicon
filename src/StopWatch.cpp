//
// Created by Haluk Akgunduz on 10/17/15.
//

#include "StopWatch.h"

void StopWatch::start() {

//    tStart = std::chrono::steady_clock::now();
    tStart = time(nullptr);

    initiated = true;
}

uint64_t StopWatch::stop() {

    if (!initiated) {
        return 0;
    }

    tStop = time(nullptr);
//    tStop = std::chrono::steady_clock::now();

//    return std::chrono::duration_cast<std::chrono::seconds>(tStop - tStart).count();
    return tStop - tStart;

}

void StopWatch::reset() {

    initiated = false;

}

bool StopWatch::isInitiated() {

    return initiated;

}