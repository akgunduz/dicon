//
// Created by Haluk Akgunduz on 10/14/15.
//

#ifndef DICON_COMPONENT_H
#define DICON_COMPONENT_H

#include "InterfaceFactory.h"
#include "DeviceList.h"
#include "MessageType.h"
#include "HostUnit.h"
#include "StopWatch.h"

enum NOTIFYSTATE {
    NOTIFYSTATE_PASSIVE,
    NOTIFYSTATE_ACTIVE,
    NOTIFYSTATE_ONCE,
    NOTIFYSTATE_TRANSPARENT,
    NOTIFYSTATE_MAX,
};

class Component;

typedef std::shared_ptr<Component> TypeComponent;
typedef std::vector<TypeComponent> TypeComponentList;
typedef std::map<COMPONENT, TypeComponentList> TypeComponentMapList;
typedef bool (Component::*TypeProcessComponentMsg)(ComponentUnit&, TypeMessage);
typedef bool (*TypeStaticProcessComponentMsg)(const TypeComponent&, ComponentUnit&, TypeMessage);

typedef std::map<const MSG_TYPE, TypeProcessComponentMsg> TypeProcessMsgMap;
typedef std::map<const MSG_TYPE, TypeStaticProcessComponentMsg> TypeStaticProcessMsgMap;

typedef bool (*TypeNotifyCB)(void*, COMPONENT, NOTIFYSTATE);

class Component : public std::enable_shared_from_this<Component> {

    TypeInterfaceList interfaces;

    static void *notifyContext;
    static TypeNotifyCB notifyCB;

protected :

    TypeHostUnit host;

    TypeProcessMsgMap processMsg[COMP_MAX];
    TypeStaticProcessMsgMap processStaticMsg[COMP_MAX];

    const InterfaceSchedulerCB *schedulerCB;

    StopWatch componentWatch;

    bool initInterfaces(COMPONENT type, int, int);

    bool notifyUI(NOTIFYSTATE);

public:

    static TypeComponent nullComponent;

    Component();
    virtual ~Component();

    TypeHostUnit& getHost();

    const TypeDevice& getDevice(COMPONENT);
    Address& getInterfaceAddress(COMPONENT);
    Address& getInterfaceMulticastAddress(COMPONENT);
    TypeAddressList getInterfaceAddressList(COMPONENT);
    INTERFACE getInterfaceType(COMPONENT);
    bool isSupportMulticast(COMPONENT);
    bool onReceive(ComponentUnit&, MSG_TYPE, TypeMessage);

    bool send(ComponentUnit&, TypeMessage);

    bool defaultProcessMsg(ComponentUnit&, TypeMessage);

    static void registerNotify(void*, TypeNotifyCB);

    bool setID(long);

    bool addProcessHandler(COMPONENT, MSG_TYPE, TypeProcessComponentMsg);
    bool addStaticProcessHandler(COMPONENT, MSG_TYPE, TypeStaticProcessComponentMsg);
};

#endif //DICON_COMPONENT_H
