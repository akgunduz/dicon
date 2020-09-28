//
// Created by Haluk Akgunduz on 10/17/15.
//

#ifndef DICON_UTILTIME_H
#define DICON_UTILTIME_H

#include "Common.h"

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
