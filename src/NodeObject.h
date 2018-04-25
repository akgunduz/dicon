//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef BANKOR_NODEOBJECT_H
#define BANKOR_NODEOBJECT_H

#include "StopWatch.h"
#include "NodeWatchdog.h"
#include "NodeState.h"

class NodeWatchdog;

class NodeObject {

private:

    NODE_STATES state;
    int usage;
    std::vector<Md5> md5List;

#ifndef DISABLE_RECOVERY
    NodeWatchdog *watchdog;
#endif

public:

    NodeObject(NODE_STATES, int);
    NodeObject();
    ~NodeObject();

    NODE_STATES getState();
    void setState(NODE_STATES);
    int getUsage();
    int iterateUsage(bool);
    std::vector<Md5>* getMD5List();

};


#endif //BANKOR_NODEOBJECT_H
