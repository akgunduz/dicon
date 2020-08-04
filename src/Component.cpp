//
// Created by Haluk Akgunduz on 10/14/15.
//

#include "Component.h"
#include "Util.h"

void *Component::notifyContext = nullptr;
TypeNotifyCB Component::notifyCB = nullptr;

Component::Component(const char *rootPath) {

    strcpy(this->rootPath, rootPath);

    schedulerCB = new InterfaceSchedulerCB([](void *arg, SchedulerItem *item) -> bool {

        auto *component = (Component *) arg;
        auto *messageItem = (MessageItem *) item;

        ComponentUnit obj(messageItem->getMessage()->getHeader().getOwner());

        return component->onReceive(obj, messageItem->getMessage()->getHeader().getType(),
                                    messageItem->getMessage());

    }, this);

    hostCB = new InterfaceHostCB([](void *arg) -> HostUnit & {

        return ((Component *) arg)->getHost();

    }, this);
}

bool Component::initInterfaces(COMPONENT type, int interfaceOther, int interfaceNode) {

    DeviceList *deviceList = DeviceList::getInstance();

    Device *nodeDevice = deviceList->get(interfaceNode);
    Device *otherDevice = deviceList->get(interfaceOther);

    interfaces[COMP_NODE] = Connector::createInterface(nodeDevice, schedulerCB, hostCB);
    interfaces[COMP_DISTRIBUTOR] = otherDevice != nodeDevice && type != COMP_NODE ?
                                   Connector::createInterface(otherDevice, schedulerCB, hostCB) :
                                   interfaces[COMP_NODE];
    interfaces[COMP_COLLECTOR] = interfaces[COMP_DISTRIBUTOR];

    host->setAddress(ComponentUnit::next(type), getInterfaceAddress(ComponentUnit::next(type)));
    host->setAddress(ComponentUnit::prev(type), getInterfaceAddress(ComponentUnit::prev(type)));

    return true;
}

Component::~Component() {

    if (interfaces[COMP_DISTRIBUTOR] != interfaces[COMP_NODE]) {
        delete interfaces[COMP_DISTRIBUTOR];
    }

    delete interfaces[COMP_NODE];

    delete schedulerCB;

    delete hostCB;
}

const char *Component::getRootPath() {

    return rootPath;
}

bool Component::onReceive(ComponentUnit &owner, MSG_TYPE msgType, Message *msg) {

    LOGC_D(getHost(), owner, MSGDIR_RECEIVE,
           "\"%s\" is received",
           MessageTypes::getMsgName(msgType));

    auto processCB = processMsg[owner.getType()].find(msgType);
    if (processCB == processMsg[owner.getType()].end()) {

        auto processStaticCB = processStaticMsg[owner.getType()].find(msgType);
        if (processStaticCB == processStaticMsg[owner.getType()].end()) {

            return defaultProcessMsg(owner, msg);
        }

        return (processStaticMsg[owner.getType()][msgType])(this, owner, msg);
    }

    return (this->*processMsg[owner.getType()][msgType])(owner, msg);
}


bool Component::defaultProcessMsg(ComponentUnit &owner, Message *msg) {

    if (getHost().getType() != owner.getType()) {
        LOGC_W(getHost(), owner, MSGDIR_RECEIVE,
               "No Handler is found for message : \"%s\"",
               MessageTypes::getMsgName(msg->getHeader().getType()));
    }
    delete msg;
    return true;
}

Device *Component::getDevice(COMPONENT target) {

    if (interfaces[target] != nullptr) {
        return interfaces[target]->getDevice();
    }

    return nullptr;
}

Address &Component::getInterfaceAddress(COMPONENT target) {

    if (interfaces[target] != nullptr) {
        return interfaces[target]->getAddress();
    }

    return Address::invalid;
}

Address &Component::getInterfaceMulticastAddress(COMPONENT target) {

    if (interfaces[target] != nullptr) {
        return interfaces[target]->getMulticastAddress();
    }

    return Address::invalid;
}

TypeAddressList Component::getInterfaceAddressList(COMPONENT target) {

    if (interfaces[target] != nullptr) {
        return interfaces[target]->getAddressList();
    }

    return TypeAddressList();
}

INTERFACE Component::getInterfaceType(COMPONENT target) {

    if (interfaces[target] != nullptr) {
        return interfaces[target]->getType();
    }

    return INTERFACE_MAX;
}


bool Component::isSupportMulticast(COMPONENT target) {

    if (interfaces[target] != nullptr) {
        return interfaces[target]->isSupportMulticast();
    }

    return false;
}

bool Component::send(ComponentUnit &target, Message *msg) {

    return interfaces[target.getType()]->push(MSGDIR_SEND, target, msg);
}

HostUnit &Component::getHost() {
    return (*host);
}

void Component::registerNotify(void *_notifyContext, TypeNotifyCB _notifyCB) {

    notifyContext = _notifyContext;
    notifyCB = _notifyCB;
}

void Component::notifyUI(NOTIFYSTATE state) {

    if (notifyContext) {
        notifyCB(notifyContext, getHost().getType(), state);
    }
}

void Component::setID(long id) {

    if (id == 0) {
        LOGS_E(getHost(), "Can not assign ID with 0!!!");
        return;
    }

    getHost().setID(id);

    int pos = strlen(rootPath);
    sprintf(&rootPath[pos], "/%ld", id);

    Util::removePath(getRootPath());
    mkdir(rootPath, 0777);
}

bool Component::addProcessHandler(COMPONENT component, MSG_TYPE msgType,
                                  TypeProcessComponentMsg handler) {

    processMsg[component][msgType] = handler;

    return true;
}

bool Component::addStaticProcessHandler(COMPONENT component, MSG_TYPE msgType,
                                        TypeStaticProcessComponentMsg handler) {

    processStaticMsg[component][msgType] = handler;

    return true;
}


