//
// Created by Haluk AKGUNDUZ on 17.10.2015.
// Copyright (c) 2015 Haluk Akgunduz. All rights reserved.
//

#include "UtilTime.h"

uint64_t UtilTime::duration(bool reset) {

    tStop = std::chrono::high_resolution_clock::now();

    uint64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(tStop - tStart).count();

    if (reset) {
        tStart = tStop;
    }

    return duration;
}

void UtilTime::reset() {

    tStart = std::chrono::high_resolution_clock::now();
}
