//
// Created by Haluk Akgunduz on 10/17/15.
//

#ifndef DICON_STOPWATCH_H
#define DICON_STOPWATCH_H

#include "Common.h"

class StopWatch {

    bool initiated;
    long long tStart{};
    long long tStop{};

public:

    StopWatch(){
        initiated = false;
    };
    void start();
    long stop();
    void reset();
    bool isInitiated();
};


#endif //DICON_STOPWATCH_H
