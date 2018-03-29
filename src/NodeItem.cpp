//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "NodeItem.h"

NodeItem::NodeItem(NODE_STATES s, int u, long a, short i) :
        state(s), usage(u), address(a), id(i) {

    watchdog = nullptr;
    lastServedCollector = 0;
    stopWatch.reset();
}

NodeItem::~NodeItem() {

    if (watchdog != nullptr) {
        delete watchdog;
    }
}
