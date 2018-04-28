//
// Created by Haluk Akgunduz on 10/14/15.
//

#include "Component.h"

Component::Component(COMPONENT host, const char* rootPath) :
    host(host), id(0) {

    ComponentTypes::setRootPath(getHost(), rootPath);

    schedulerCB = new InterfaceSchedulerCB(receiveCB, this);
    hostCB = new InterfaceHostCB(getHostCB, getIDCB, this);

    DeviceList *deviceList = DeviceList::getInstance();

    interfaces[COMP_NODE] = Connector::createInterface(deviceList->getActive(1), schedulerCB, hostCB);
    interfaces[COMP_DISTRIBUTOR] = deviceList->isActiveDifferent() && host != COMP_NODE ?
                                   Connector::createInterface(deviceList->getActive(0), schedulerCB, hostCB) :
                                   interfaces[COMP_NODE];
    interfaces[COMP_COLLECTOR] = interfaces[COMP_DISTRIBUTOR];

}

Component::~Component() {

    if (interfaces[COMP_DISTRIBUTOR] != interfaces[COMP_NODE]) {
        delete interfaces[COMP_DISTRIBUTOR];
    }

    delete interfaces[COMP_NODE];

    delete schedulerCB;

    delete hostCB;
}

COMPONENT Component::getHost() {

    return host;
}

int Component::getHostCB(void *arg) {

    return (int)((Component*) arg)->getHost();
}

int Component::getID() {

    return id;
}

int Component::getIDCB(void *arg) {

    return ((Component*) arg)->getID();
}

void Component::setID(int id) {

    this->id = id;
}

bool Component::receiveCB(void *arg, SchedulerItem* item) {

    Component *component = (Component *) arg;
    MessageItem *messageItem = (MessageItem*) item;

    return component->onReceive(messageItem->address, messageItem->msg);
}

bool Component::onReceive(long address, Message *msg) {

    LOGC_I(getHost(),
           getID(),
           msg->getHeader()->getOwner(),
           AddressHelper::getID(address),
          false,
          "\"%s\" is received",
          MessageTypes::getName(msg->getHeader()->getType()));

    if (msg->getHeader()->getOwner() >= COMP_MAX) {

        LOGS_E(getHost(), getID(), "Wrong message received : %d from %s, disgarding",
              msg->getHeader()->getType(),
              InterfaceTypes::getAddressString(address).c_str());

        delete msg;
        return false;
    }

    auto processCB = processMsg[msg->getHeader()->getOwner()].find(msg->getHeader()->getType());
    if (processCB == processMsg[msg->getHeader()->getOwner()].end()) {

        return defaultProcessMsg(address, msg);
    }

    return (this->*processMsg[msg->getHeader()->getOwner()][msg->getHeader()->getType()])(address, msg);
}


bool Component::defaultProcessMsg(long address, Message *msg) {

    delete msg;
    return true;
}


long Component::getInterfaceAddress(COMPONENT target) {

    if (interfaces[target] != NULL) {
        return interfaces[target]->getAddress();
    }

    return 0;
}


INTERFACE Component::getInterfaceType(COMPONENT target) {

    if (interfaces[target] != NULL) {
        return interfaces[target]->getType();
    }

    return INTERFACE_MAX;
}


bool Component::isSupportMulticast(COMPONENT target) {

    if (interfaces[target] != NULL) {
        return interfaces[target]->isSupportMulticast();
    }

    return false;
}

bool Component::send(COMPONENT target, long address, Message *msg) {

    LOGC_I(getHost(),
           getID(),
           target,
           AddressHelper::getID(address),
           true,
           "\"%s\" is sent",
           MessageTypes::getName(msg->getHeader()->getType()));

    msg->getHeader()->setOwnerAddress(AddressHelper::setID(interfaces[target]->getAddress(), getID()));

    return interfaces[target]->push(MESSAGE_SEND, address, msg);
}

bool Component::send(COMPONENT target, Message *msg) {

    LOGS_I(getHost(),
           getID(),
           "Multicast \"%s\" is sent",
           MessageTypes::getName(msg->getHeader()->getType()));

    msg->getHeader()->setOwnerAddress(AddressHelper::setID(interfaces[target]->getAddress(), getID()));

    return interfaces[target]->push(MESSAGE_SEND, interfaces[target]->getMulticastAddress(), msg);
}

bool Component::put(COMPONENT target, long address, Message *msg) {

    return interfaces[target]->push(MESSAGE_RECEIVE, address, msg);
}

std::vector<long> Component::getAddressList(COMPONENT target) {

    return interfaces[target]->getDevice()->getAddressList();
}

