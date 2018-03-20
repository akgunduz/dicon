//
// Created by Haluk Akgunduz on 10/14/15.
//

#ifndef BANKOR_COMPONENT_H
#define BANKOR_COMPONENT_H

#include "Connector.h"
#include "MessageTypes.h"

class Component {

protected :

    InterfaceCallback *callback;

    Unit host;

    void setHost(Unit);

public:
    Connector *connectors[COMP_MAX];

    Component(Unit host, const char* rootPath);
    virtual ~Component();
    Unit getHost();
    long getAddress(COMPONENT host);
    static bool receiveCB(void *, SchedulerItem*);
    bool onReceive(long, Message *);

    virtual bool processDistributorMsg(long, Message *) = 0;
    virtual bool processCollectorMsg(long, Message *) = 0;
    virtual bool processNodeMsg(long, Message *) = 0;

};


#endif //BANKOR_COMPONENT_H
