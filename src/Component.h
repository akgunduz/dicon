//
// Created by Haluk Akgunduz on 10/14/15.
//

#ifndef DICON_COMPONENT_H
#define DICON_COMPONENT_H

#include "Connector.h"
#include "DeviceList.h"
#include "MessageTypes.h"
#include "ComponentObject.h"
#include "StopWatch.h"

enum NOTIFYSTATE {
    NOTIFYSTATE_PASSIVE,
    NOTIFYSTATE_ACTIVE,
    NOTIFYSTATE_LOAD,
    NOTIFYSTATE_TRANSPARENT,
    NOTIFYSTATE_MAX,
};

class Component;

typedef bool (Component::*TypeProcessComponentMsg)(const ComponentObject&, Message *);
typedef bool (*TypeStaticProcessComponentMsg)(Component*, const ComponentObject&, Message *);

typedef std::map<const MSG_TYPE, TypeProcessComponentMsg> TypeProcessMsgMap;
typedef std::map<const MSG_TYPE, TypeStaticProcessComponentMsg> TypeStaticProcessMsgMap;

typedef bool (*TypeNotifyCB)(void*, COMPONENT, NOTIFYSTATE);

class Component {

    char rootPath[PATH_MAX]{};

    Interface *interfaces[COMP_MAX]{};

    static void *notifyContext;
    static TypeNotifyCB notifyCB;

protected :

    ComponentObject *host{};

    TypeProcessMsgMap processMsg[COMP_MAX];
    TypeStaticProcessMsgMap processStaticMsg[COMP_MAX];

    const InterfaceSchedulerCB *schedulerCB;
    const InterfaceHostCB *hostCB;

    StopWatch componentWatch;

    static ComponentObject& getHostCB(void*);

    bool initInterfaces(COMPONENT type, int, int);

    void notifyUI(NOTIFYSTATE);

public:

    explicit Component(const char* rootPath);
    virtual ~Component();

    ComponentObject& getHost();

    const char* getRootPath();

    Device* getDevice(COMPONENT);
    long getInterfaceAddress(COMPONENT);
    long getInterfaceMulticastAddress(COMPONENT);
    INTERFACE getInterfaceType(COMPONENT);
    bool isSupportMulticast(COMPONENT);
    static bool receiveCB(void *, SchedulerItem*);
    bool onReceive(const ComponentObject&, MSG_TYPE, Message *);

    bool send(const ComponentObject&, Message*);

    bool defaultProcessMsg(const ComponentObject&, Message *);

    static void registerNotify(void*, TypeNotifyCB);

    void setID(long);

    bool addProcessHandler(COMPONENT, MSG_TYPE, TypeProcessComponentMsg);
    bool addStaticProcessHandler(COMPONENT, MSG_TYPE, TypeStaticProcessComponentMsg);

};


#endif //DICON_COMPONENT_H
