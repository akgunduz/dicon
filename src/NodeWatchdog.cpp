//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "NodeWatchdog.h"

NodeWatchdog::NodeWatchdog(Component *component, fWakeupCB wakeupCB) {

    this->component = component;
    this->wakeupCB = wakeupCB;
    this->timerType = TIMER_WAKEUP;
    this->timeout = WAKEUP_TIMEOUT;
    init();
}

NodeWatchdog::NodeWatchdog(Component *component, NodeObject *nodeItem, fTimeoutCB timeoutCB) {

    this->component = component;
    this->timeoutCB = timeoutCB;
    this->timerType = TIMER_NODE;
    this->nodeItem = nodeItem;
    this->timeout = NODE_TIMEOUT;
    init();
}

void NodeWatchdog::init() {

    thread = std::thread(onTimerThread, this);
}

void NodeWatchdog::onTimerThread(NodeWatchdog *nodeWatchdog) {

    while(true) {

        auto now = std::chrono::system_clock::now();

        std::unique_lock<std::mutex> lock(nodeWatchdog->mutex);

        if (nodeWatchdog->cond.wait_until(lock, now + std::chrono::seconds(nodeWatchdog->timeout)) !=
            std::cv_status::timeout) {

            if (nodeWatchdog->timerType == TIMER_WAKEUP) {
                nodeWatchdog->wakeupCB(nodeWatchdog->component);

            } else {
                nodeWatchdog->timeoutCB(nodeWatchdog->component, nodeWatchdog->nodeItem);
                break;
            }

        } else {
            break;
        }

        lock.unlock();

    }
}

void NodeWatchdog::end() {

    std::unique_lock<std::mutex> lock(mutex);
    cond.notify_one();
    lock.unlock();
    thread.join();
}

NodeWatchdog::~NodeWatchdog() {

    end();
}