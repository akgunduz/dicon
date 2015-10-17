//
// Created by Haluk Akgunduz on 10/17/15.
//

#ifndef BANKOR_STOPWATCH_H
#define BANKOR_STOPWATCH_H

#include "Common.h"

class StopWatch {

    bool initiated = false;
    double tStart;
    double tStop;

public:

    void start();
    double stop();
    void reset();
    bool isInitiated();
};


#endif //BANKOR_STOPWATCH_H
