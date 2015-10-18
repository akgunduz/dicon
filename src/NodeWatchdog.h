//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef BANKOR_NODEWATCHDOG_H
#define BANKOR_NODEWATCHDOG_H

#include "Connector.h"
#include "NodeItem.h"
#include "Component.h"

#define NODE_TIMEOUT 10
#define WAKEUP_TIMEOUT 30

class NodeItem;

typedef bool (*fTimeoutCB)(Component*, NodeItem*);
typedef bool (*fWakeupCB)(Component*);

enum TimerType {
    TIMER_NODE,
    TIMER_WAKEUP
};

class NodeWatchdog {

    TimerType timerType;
    NodeItem *nodeItem;
    int timeout;

    pthread_mutex_t mutex;
    pthread_cond_t condition;
    pthread_t thread;

    Component *component;
    fTimeoutCB timeoutCB;
    fWakeupCB wakeupCB;

    void init();
    static void *onTimerThread(void *);

public:

    NodeWatchdog(Component *component, fWakeupCB);
    NodeWatchdog(Component *component, NodeItem*, fTimeoutCB);
    ~NodeWatchdog();

    void end();
};


#endif //BANKOR_NODEWATCHDOG_H
