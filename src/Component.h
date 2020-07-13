//
// Created by Haluk Akgunduz on 10/14/15.
//

#ifndef DICON_COMPONENT_H
#define DICON_COMPONENT_H

#include "Connector.h"
#include "DeviceList.h"
#include "MessageTypes.h"
#include "ComponentObject.h"
#include "Timer.h"

class Component;

typedef bool (Component::*TypeProcessComponentMsg)(const ComponentObject&, Message *);

typedef std::map<const MSG_TYPE, TypeProcessComponentMsg> TypeProcessMsgMap;

class Component {

    char rootPath[PATH_MAX];

    Interface *interfaces[COMP_MAX];

    static void *notifyContext;
    static TypeNotifyCB notifyCB;

protected :

    ComponentObject *host;

    TypeProcessMsgMap processMsg[COMP_MAX];

    const InterfaceSchedulerCB *schedulerCB;
    const InterfaceHostCB *hostCB;

    static ComponentObject getHostCB(void*);

    bool initInterfaces(COMPONENT type);

    void notifyUI();

public:

    explicit Component(const char* rootPath);
    virtual ~Component();

    ComponentObject& getHost();

    const char* getRootPath();

    long getInterfaceAddress(ComponentObject);
    long getInterfaceMulticastAddress(ComponentObject);
    INTERFACE getInterfaceType(ComponentObject);
    bool isSupportMulticast(ComponentObject);
    static bool receiveCB(void *, SchedulerItem*);
    bool onReceive(ComponentObject, MSG_TYPE, Message *);

    //bool send(ComponentObject, long, Message*);
    bool send(ComponentObject, Message*);

    std::vector<long> getAddressList(const ComponentObject&);
    bool defaultProcessMsg(ComponentObject, Message *);

    static void registerNotify(void*, TypeNotifyCB);

    bool isIDAssigned();

    void setID(int);

};


#endif //DICON_COMPONENT_H
