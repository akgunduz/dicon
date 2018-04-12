//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef BANKOR_NODEOBJECT_H
#define BANKOR_NODEOBJECT_H

#include "Common.h"
#include "StopWatch.h"
#include "NodeWatchdog.h"

class NodeWatchdog;

class NodeObject {

private:

    NODE_STATES state;
    int usage;
    NodeInfo nodeInfo;
    std::vector<Md5> md5List;

#ifndef DISABLE_RECOVERY
    NodeWatchdog *watchdog;
#endif

public:

    NodeObject(NODE_STATES, int, NodeInfo);
    NodeObject(NodeInfo);
    ~NodeObject();

    NODE_STATES getState();
    void setState(NODE_STATES);
    int getUsage();
    void iterateUsage(bool);
    NodeInfo* getNode();
    std::vector<Md5>* getMD5List();

};


#endif //BANKOR_NODEOBJECT_H
