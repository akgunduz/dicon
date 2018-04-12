//
// Created by Haluk Akgunduz on 10/14/15.
//

#ifndef BANKOR_COMPONENT_H
#define BANKOR_COMPONENT_H

#include "Connector.h"
#include "DeviceList.h"
#include "MessageTypes.h"

class Component {

    COMPONENT host;

    Interface *interfaces[COMP_MAX];

protected :

    InterfaceCallback *callback;

    void setHost(COMPONENT);

public:

    Component(COMPONENT host, const char* rootPath);
    virtual ~Component();
    COMPONENT getHost();
    long getInterfaceAddress(COMPONENT);
    INTERFACE getInterfaceType(COMPONENT);
    bool isSupportMulticast(COMPONENT);
    static bool receiveCB(void *, SchedulerItem*);
    bool onReceive(long, Message *);

    bool send(COMPONENT, long, Message*);
    bool send(COMPONENT, Message*);
    bool put(COMPONENT, long, Message*);
    std::vector<long> getAddressList(COMPONENT);

    virtual bool processDistributorMsg(long, Message *) = 0;
    virtual bool processCollectorMsg(long, Message *) = 0;
    virtual bool processNodeMsg(long, Message *) = 0;

};


#endif //BANKOR_COMPONENT_H
