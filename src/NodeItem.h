//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef BANKOR_NODEITEM_H
#define BANKOR_NODEITEM_H

#include "Common.h"
#include "StopWatch.h"
#include "NodeWatchdog.h"

class NodeWatchdog;

class NodeItem {

public:

    NodeStates state = IDLE;
    int usage;
    long address;
    long lastServedCollector;
    short id;
    StopWatch stopWatch;
    NodeWatchdog *watchdog;

    NodeItem(NodeStates s, int u, long a, short i);
    ~NodeItem();

};


#endif //BANKOR_NODEITEM_H
