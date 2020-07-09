//
// Created by akgun on 2.07.2020.
//

#include "Timer.h"

#define TIMER_GRANUL 10

TypeTimerList Timer::timerList;

Timer::Timer(int id, long timeout, TypeNotifyCB cb, void* context, int cbData)
    : id(id), timeout(timeout), cb(cb), context(context), cbData(cbData) {

    thread = std::thread(run, this);
}

void *Timer::run(void *arg) {

    auto *timer = (Timer *) arg;

    do {

        timer->reTrigger = false;

        timer->curr = timer->timeout;

        while (timer->curr) {

            std::this_thread::sleep_for(std::chrono::milliseconds(TIMER_GRANUL));

            timer->curr -= TIMER_GRANUL;
        }

        timer->cb(timer->context, timer->id, timer->cbData);

    } while(timer->reTrigger);

    timerList.erase(timer->id);

    return nullptr;
}

void Timer::reset(long ms) {

    timeout = ms;
    reTrigger = true;
}

Timer *Timer::set(int id, long ms, TypeNotifyCB cb, void* context, int cbData) {

    auto timer = Timer::timerList.find(id);
    if (timer == Timer::timerList.end()) {
        timerList[id] = new Timer(id, ms, cb, context, cbData);
        return timerList[id];
    }
    timer->second->reset(ms);
    return timer->second;
}
