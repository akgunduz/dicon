//
// Created by akgun on 2.07.2020.
//

#include "Timer.h"

#define TIMER_GRANUL 10

TypeTimerList Timer::timerList;

Timer::Timer(std::string name, long timeout, TypeTimerCB cb, void* cbData)
    : name(name), timeout(timeout), cb(cb), cbData(cbData) {

    thread = std::thread(run, this);
}

void *Timer::run(void *arg) {

    Timer *timer = (Timer *) arg;

    do {

        timer->retrigger = false;

        timer->curr = timer->timeout;

        while (timer->curr) {

            usleep(TIMER_GRANUL * 1000);

            timer->curr -= TIMER_GRANUL;
        }

        timer->cb(timer->cbData, timer->name.c_str());

    } while(timer->retrigger);

    timerList.erase(timer->name);

    return nullptr;
}

void Timer::reset(long ms) {

    timeout = ms;
    retrigger = true;
}

Timer *Timer::set(std::string name, long ms, TypeTimerCB cb, void *cbData) {

    auto timer = Timer::timerList.find(name);
    if (timer == Timer::timerList.end()) {
        timerList[name] = new Timer(name, ms, cb, cbData);
        return timerList[name];
    }
    timer->second->reset(ms);
    return timer->second;
}

//Timer *Timer::create(std::string name, long ms, TypeTimerCB cb, void *cbData) {
//
//    timerList[name] = new Timer(ms, cb, cbData);
//    return timerList[name];
//}
