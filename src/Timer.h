//
// Created by akgun on 2.07.2020.
//

#ifndef DICON_TIMER_H
#define DICON_TIMER_H

#include "Common.h"

class Timer;

typedef bool (*TypeNotifyCB)(void*, int);
typedef std::map<int, Timer*> TypeTimerList;

class Timer {

    std::thread thread;
    int id;
    long timeout;
    long curr;
    bool reTrigger;

    TypeNotifyCB cb;
    void* cbData;
    static TypeTimerList timerList;
    static void* run(void *);
    Timer(int, long, TypeNotifyCB, void*);

public:
    static Timer* set(int, long ms, TypeNotifyCB cb, void *cbData);

    void reset(long);
};


#endif //DICON_TIMER_H
