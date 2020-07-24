//
// Created by Haluk Akgunduz on 10/17/15.
//

#ifndef DICON_STOPWATCH_H
#define DICON_STOPWATCH_H

#include "Common.h"

class StopWatch {

    bool initiated;
    double tStart{};
    double tStop{};

public:

    StopWatch(){
        initiated = false;
    };
    void start();
    double stop();
    void reset();
    bool isInitiated();
};


#endif //DICON_STOPWATCH_H
