//
// Created by akgun on 2.07.2020.
//

#ifndef DICON_TIMER_H
#define DICON_TIMER_H

#include "Common.h"

class Timer;

typedef bool (*TypeNotifyCB)(void*, int, int);
typedef std::map<int, Timer*> TypeTimerList;

class Timer {

    std::thread thread;
    int id;
    long timeout;
    long curr;
    bool reTrigger;

    TypeNotifyCB cb;
    void* context;
    int cbData;
    static TypeTimerList timerList;
    static void* run(void *);
    Timer(int, long, TypeNotifyCB, void*, int);

public:
    static Timer* set(int, long, TypeNotifyCB, void*, int);

    void reset(long);
};


#endif //DICON_TIMER_H
