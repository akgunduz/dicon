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

    int res = pthread_mutex_init(&mutex, NULL);
    if (res) {
        LOG_E("NodeWatchdog --> Mutex init failed");
        return;
    }

    res = pthread_cond_init(&condition, NULL);
    if (res) {
        LOG_E("NodeWatchdog --> Condition init fail");
        pthread_mutex_destroy(&mutex);
        return;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    res = pthread_create(&thread, &attr, onTimerThread, (void *)this);
    pthread_attr_destroy(&attr);
    if (res) {
        LOG_E("NodeWatchdog --> Thread init fail");
        pthread_cond_destroy(&condition);
        pthread_mutex_destroy(&mutex);
    }
}

void *NodeWatchdog::onTimerThread(void *ptr) {

    NodeWatchdog* nodeWatchdog = (NodeWatchdog*) ptr;

    struct timespec timer;
    struct timeval now;

    while(1) {

        gettimeofday(&now, NULL);

        timer.tv_sec = now.tv_sec + nodeWatchdog->timeout;
        timer.tv_nsec = 0;

        pthread_mutex_lock(&nodeWatchdog->mutex);
        int res = pthread_cond_timedwait(&nodeWatchdog->condition,
                                         &nodeWatchdog->mutex, &timer);
        if (res == ETIMEDOUT) {

            if (nodeWatchdog->timerType == TIMER_WAKEUP) {
                nodeWatchdog->wakeupCB(nodeWatchdog->component);

            } else {
                nodeWatchdog->timeoutCB(nodeWatchdog->component, nodeWatchdog->nodeItem);
                break;
            }

        } else {
            break;
        }

        pthread_mutex_unlock(&nodeWatchdog->mutex);

    }
    pthread_cond_destroy(&nodeWatchdog->condition);
    pthread_mutex_destroy(&nodeWatchdog->mutex);

    return nullptr;
}

void NodeWatchdog::end() {

    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&condition);
    pthread_mutex_unlock(&mutex);
    pthread_join(thread, nullptr);
}

NodeWatchdog::~NodeWatchdog() {

    end();
}