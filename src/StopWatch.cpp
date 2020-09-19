//
// Created by Haluk Akgunduz on 10/17/15.
//

#include "StopWatch.h"

void StopWatch::start() {

    tStart = std::chrono::steady_clock::now();

    initiated = true;
}

std::chrono::seconds StopWatch::stop() {

    if (!initiated) {
        return std::chrono::seconds(0);
    }

    tStop = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::seconds>(tStop - tStart);

}

void StopWatch::reset() {

    initiated = false;

}

bool StopWatch::isInitiated() {

    return initiated;

}