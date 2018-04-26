//
// Created by Haluk Akgunduz on 10/14/15.
//

#ifndef BANKOR_COMPONENT_H
#define BANKOR_COMPONENT_H

#include "Connector.h"
#include "DeviceList.h"
#include "MessageTypes.h"

class Component;

typedef bool (Component::*TypeProcessComponentMsg)(long, Message *);

typedef std::map<const MSG_TYPE, TypeProcessComponentMsg> TypeProcessMsgMap;

class Component {

    COMPONENT host;

    int id;

    Interface *interfaces[COMP_MAX];

protected :

    TypeProcessMsgMap processMsg[COMP_MAX];

    InterfaceCallback *callback;

    void setHost(COMPONENT);

public:

    Component(COMPONENT host, const char* rootPath);
    virtual ~Component();

    COMPONENT getHost();

    int getID();
    void setID(int);

    long getInterfaceAddress(COMPONENT);
    INTERFACE getInterfaceType(COMPONENT);
    bool isSupportMulticast(COMPONENT);
    static bool receiveCB(void *, SchedulerItem*);
    bool onReceive(long, Message *);

    bool send(COMPONENT, long, Message*);
    bool send(COMPONENT, Message*);
    bool put(COMPONENT, long, Message*);
    std::vector<long> getAddressList(COMPONENT);
    bool defaultProcessMsg(long, Message *);

};


#endif //BANKOR_COMPONENT_H
