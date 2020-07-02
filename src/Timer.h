//
// Created by akgun on 2.07.2020.
//

#ifndef DICON_TIMER_H
#define DICON_TIMER_H

#include "Common.h"

class Timer;

typedef bool (*TypeTimerCB)(void*, const char*);
typedef std::map<std::string, Timer*, char_cmp> TypeTimerList;

class Timer {

    std::thread thread;
    std::string name;
    long timeout;
    long curr;
    bool retrigger;

    TypeTimerCB cb;
    void* cbData;
    static TypeTimerList timerList;
    static void* run(void *);
    Timer(std::string, long, TypeTimerCB, void*);

public:
    static Timer* set(std::string, long ms, TypeTimerCB cb, void *cbData);

    void reset(long);
};


#endif //DICON_TIMER_H
