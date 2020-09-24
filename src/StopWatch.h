//
// Created by Haluk Akgunduz on 10/17/15.
//

#ifndef DICON_STOPWATCH_H
#define DICON_STOPWATCH_H

#include "Common.h"

typedef uint64_t TypeTime;
//typedef std::chrono::steady_clock::time_point TypeTime;

class StopWatch {

    bool initiated;
    TypeTime tStart{};
    TypeTime tStop{};

public:

    StopWatch(){
        initiated = false;
    };
    void start();
    uint64_t stop();
    void reset();
    bool isInitiated();
};


#endif //DICON_STOPWATCH_H
