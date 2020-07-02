//
// Created by Haluk Akgunduz on 10/14/15.
//

#ifndef DICON_COMPONENT_H
#define DICON_COMPONENT_H

#include "Connector.h"
#include "DeviceList.h"
#include "MessageTypes.h"
#include "ComponentObject.h"

class Component;

typedef bool (Component::*TypeProcessComponentMsg)(ComponentObject, Message *);

typedef std::map<const MSG_TYPE, TypeProcessComponentMsg> TypeProcessMsgMap;

class Component {

    ComponentObject host;

    char rootPath[PATH_MAX];

    Interface *interfaces[COMP_MAX];

protected :

    TypeProcessMsgMap processMsg[COMP_MAX];

    const InterfaceSchedulerCB *schedulerCB;
    const InterfaceHostCB *hostCB;

    static ComponentObject getHostCB(void*);

public:

    Component(COMPONENT host, const char* rootPath);
    virtual ~Component();

    ComponentObject getHost();
    int getHostID();
    void setHostID(int);

    const char* getRootPath();

    long getInterfaceAddress(ComponentObject);
    long getInterfaceMulticastAddress(ComponentObject);
    INTERFACE getInterfaceType(ComponentObject);
    bool isSupportMulticast(ComponentObject);
    static bool receiveCB(void *, SchedulerItem*);
    bool onReceive(ComponentObject, MSG_TYPE, Message *);

    //bool send(ComponentObject, long, Message*);
    bool send(ComponentObject, Message*);

    std::vector<long> getAddressList(ComponentObject);
    bool defaultProcessMsg(ComponentObject, Message *);

};


#endif //DICON_COMPONENT_H
