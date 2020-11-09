//
// Created by Haluk AKGUNDUZ on 17.10.2015.
// Copyright (c) 2015 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_UTILTIME_H
#define DICON_UTILTIME_H

#include <chrono>

typedef std::chrono::high_resolution_clock::time_point TypeTime;
typedef uint64_t TypeTimeDuration;

class UtilTime {

    TypeTime tStart{};
    TypeTime tStop{};

public:

    UtilTime(){
        tStart = std::chrono::high_resolution_clock::now();
    };

    TypeTimeDuration duration(bool = false);
    void reset();
};


#endif //DICON_UTILTIME_H
