//
// Created by akgun on 2.07.2020.
//

#ifndef DICON_TIMER_H
#define DICON_TIMER_H

#include "Common.h"

class Timer;

typedef bool (*TypeNotifyCB)(void*, long, long);
typedef std::map<int, Timer*> TypeTimerList;

class Timer {

    std::thread thread;
    long id{};
    long timeout{};
    long curr{};
    bool reTrigger{};

    TypeNotifyCB cb{};
    void* context{};
    long cbData{};
    static TypeTimerList timerList;
    static void* run(void *);
    Timer(long, long, TypeNotifyCB, void*, long);

public:
    static Timer* set(long, long, TypeNotifyCB, void*, long);

    void reset(long);
};


#endif //DICON_TIMER_H
