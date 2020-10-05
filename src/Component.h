//
// Created by Haluk Akgunduz on 10/14/15.
//

#ifndef DICON_COMPONENT_H
#define DICON_COMPONENT_H

#include "Message.h"
#include "NotifyType.h"
#include "CommInterface.h"

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

    TypeInterfaceMap interfaceMap;
    TypeInterfaceList interfaceList;

    static inline void *notifyContext{};
    static inline TypeNotifyCB notifyCB{};

protected :

    TypeHostUnit host;

    bool initialized{false};

    TypeProcessMsgMap processMsg[COMP_MAX];

    TypeStaticProcessMsgMap processStaticMsg[COMP_MAX];

    const CommInterfaceCB *receiverCB;

    UtilTime timer;

    bool initInterfaces(COMPONENT type, int, int);

    bool notifyUI(NOTIFYTYPE);

public:

    static inline TypeComponent nullComponent;

    explicit Component(TypeHostUnit);
    virtual ~Component();

    TypeHostUnit& getHost();

    const TypeDevice& getDevice(COMPONENT);
    TypeAddress& getInterfaceAddress(COMPONENT);
    TypeAddress& getInterfaceMulticastAddress(COMPONENT);
    TypeAddressList getInterfaceAddressList(COMPONENT);
    COMM_INTERFACE getInterfaceType(COMPONENT);
    bool isSupportMulticast(COMPONENT);

    template <class T>
    void setUIAddress(COMPONENT _out, T _address) {

        getInterfaceAddress(_out)->setUI(_address);
    }

    template <class T>
    void setAllUIAddress(T _address) {

        getInterfaceAddress(COMP_DISTRIBUTOR)->setUI(_address);
        getInterfaceAddress(COMP_COLLECTOR)->setUI(_address);
        getInterfaceAddress(COMP_NODE)->setUI(_address);
    }

    bool onReceive(const TypeComponentUnit&, MSG_TYPE, TypeMessage);

    bool send(const TypeComponentUnit&, TypeMessage);

    bool sendShutdownMsg(const TypeComponentUnit&);
    bool defaultProcessMsg(const TypeComponentUnit&, TypeMessage);

    static void registerNotify(void*, TypeNotifyCB);
    static void deRegisterNotify();

    bool setID(TypeID&);
    bool isInitialized();

    bool addProcessHandler(COMPONENT, MSG_TYPE, TypeProcessComponentMsg);
    bool addStaticProcessHandler(COMPONENT, MSG_TYPE, TypeStaticProcessComponentMsg);
};

#endif //DICON_COMPONENT_H
