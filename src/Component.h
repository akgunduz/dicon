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
#include "NotifyType.h"

class Component;

typedef std::shared_ptr<Component> TypeComponent;
typedef std::vector<TypeComponent> TypeComponentList;
typedef std::map<COMPONENT, TypeComponentList> TypeComponentMapList;
typedef bool (Component::*TypeProcessComponentMsg)(const TypeComponentUnit&, TypeMessage);
typedef bool (*TypeStaticProcessComponentMsg)(const TypeComponent&, const TypeComponentUnit&, TypeMessage);

typedef std::map<const MSG_TYPE, TypeProcessComponentMsg> TypeProcessMsgMap;
typedef std::map<const MSG_TYPE, TypeStaticProcessComponentMsg> TypeStaticProcessMsgMap;

typedef bool (*TypeNotifyCB)(void*, COMPONENT, NOTIFYTYPE);

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

    bool notifyUI(NOTIFYTYPE);

public:

    static TypeComponent nullComponent;

    Component();
    virtual ~Component();

    TypeHostUnit& getHost();

    const TypeDevice& getDevice(COMPONENT);
    Address& getInterfaceAddress(COMPONENT);
    Address& getInterfaceMulticastAddress(COMPONENT);
    TypeAddressList getInterfaceAddressList(COMPONENT);
    COMM_INTERFACE getInterfaceType(COMPONENT);
    bool isSupportMulticast(COMPONENT);
    bool onReceive(const TypeComponentUnit&, MSG_TYPE, TypeMessage);

    bool send(const TypeComponentUnit&, TypeMessage);

    bool defaultProcessMsg(const TypeComponentUnit&, TypeMessage);

    static void registerNotify(void*, TypeNotifyCB);
    static void deRegisterNotify();

    bool setID(long);

    bool addProcessHandler(COMPONENT, MSG_TYPE, TypeProcessComponentMsg);
    bool addStaticProcessHandler(COMPONENT, MSG_TYPE, TypeStaticProcessComponentMsg);
};

#endif //DICON_COMPONENT_H
