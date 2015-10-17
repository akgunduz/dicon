//
// Created by Haluk Akgunduz on 10/17/15.
//

#include "StopWatch.h"


void StopWatch::start() {

    struct timeval st;
    int res = gettimeofday(&st, nullptr);
    if (res != 0) {
        return;
    }

    tStart = st.tv_sec + (double)st.tv_usec / 1000000;

    initiated = true;
}

double StopWatch::stop() {

    if (!initiated) {
        return 0;
    }

    struct timeval st;
    int res = gettimeofday(&st, nullptr);
    if (res != 0) {
        return 0;
    }

    tStop = st.tv_sec + (double)st.tv_usec / 1000000;

    return tStop - tStart;

}

void StopWatch::reset() {

    initiated = false;

}

bool StopWatch::isInitiated() {

    return initiated;

}