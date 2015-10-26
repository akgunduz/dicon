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

    const char* rootPath;

public:
    Connector *connectors[HOST_MAX];

    Component(Unit host, long index, const char* rootPath);
    virtual ~Component();
    const char* getRootPath();
    INTERFACES getInterfaceType(HOST host);
    long getAddress(HOST host);
    static bool receiveCB(void *, SchedulerItem*);
    bool onReceive(long, Message *);
    static long generateIndex(int indexFirst, int indexSecond, int indexThird);

    virtual bool processDistributorMsg(long, Message *) = 0;
    virtual bool processCollectorMsg(long, Message *) = 0;
    virtual bool processNodeMsg(long, Message *) = 0;

};


#endif //BANKOR_COMPONENT_H
