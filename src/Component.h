//
// Created by Haluk Akgunduz on 10/14/15.
//

#ifndef BANKOR_COMPONENT_H
#define BANKOR_COMPONENT_H

#include "Connector.h"
#include "DeviceList.h"
#include "MessageTypes.h"
#include "ComponentObject.h"

class Component;

typedef bool (Component::*TypeProcessComponentMsg)(ComponentObject, long, Message *);

typedef std::map<const MSG_TYPE, TypeProcessComponentMsg> TypeProcessMsgMap;

class Component {

    ComponentObject host;

    Interface *interfaces[COMP_MAX];

protected :

    TypeProcessMsgMap processMsg[COMP_MAX];

    const InterfaceSchedulerCB *schedulerCB;
    const InterfaceHostCB *hostCB;

    static ComponentObject getHostCB(void*);

public:

    Component(ComponentObject host, const char* rootPath);
    virtual ~Component();

    ComponentObject getHost();
    void setHostID(int);

    long getInterfaceAddress(COMPONENT);
    INTERFACE getInterfaceType(COMPONENT);
    bool isSupportMulticast(COMPONENT);
    static bool receiveCB(void *, SchedulerItem*);
    bool onReceive(ComponentObject, long, MSG_TYPE, Message *);

    bool send(ComponentObject, long, Message*);
    bool send(ComponentObject, Message*);

    std::vector<long> getAddressList(ComponentObject);
    bool defaultProcessMsg(ComponentObject, long, Message *);

};


#endif //BANKOR_COMPONENT_H
