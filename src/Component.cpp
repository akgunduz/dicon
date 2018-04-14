//
// Created by Haluk Akgunduz on 10/14/15.
//

#include "Component.h"

Component::Component(COMPONENT host, const char* rootPath) {

    setHost(host);
    ComponentTypes::setRootPath(getHost(), rootPath);

    callback = new InterfaceCallback(receiveCB, this);

    DeviceList *deviceList = DeviceList::getInstance();

    interfaces[COMP_NODE] = Connector::createInterface(host, deviceList->getActive(1), callback);
    interfaces[COMP_DISTRIBUTOR] = deviceList->isActiveDifferent() && host != COMP_NODE ?
                                   Connector::createInterface(host, deviceList->getActive(0), callback) :
                                   interfaces[COMP_NODE];
    interfaces[COMP_COLLECTOR] = interfaces[COMP_DISTRIBUTOR];

}

Component::~Component() {

    if (interfaces[COMP_DISTRIBUTOR] != interfaces[COMP_NODE]) {
        delete interfaces[COMP_DISTRIBUTOR];
    }

    delete interfaces[COMP_NODE];

    delete callback;
}

COMPONENT Component::getHost() {

    return host;
}

void Component::setHost(COMPONENT host) {

    this->host = host;
}

bool Component::receiveCB(void *arg, SchedulerItem* item) {

    Component *component = (Component *) arg;
    MessageItem *messageItem = (MessageItem*) item;

    return component->onReceive(messageItem->address, messageItem->msg);
}

bool Component::onReceive(long address, Message *msg) {

    LOG_U(ComponentTypes::getAssignedUILog(getHost()),
          "Receive --> \"%s\" from %s: at %s",
          MessageTypes::getName(msg->getHeader()->getType()),
          ComponentTypes::getName(msg->getHeader()->getOwner()),
          InterfaceTypes::getAddressString(address).c_str());

    switch(msg->getHeader()->getOwner()) {

        case COMP_DISTRIBUTOR:
            return processDistributorMsg(address, msg);

        case COMP_NODE:
            return processNodeMsg(address, msg);

        case COMP_COLLECTOR:
            return processCollectorMsg(address, msg);

        default:
            LOG_W("Wrong message received : %d from %s, disgarding",
                  msg->getHeader()->getType(),
                  InterfaceTypes::getAddressString(address).c_str());
            delete msg;
            return false;
    }
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

    LOG_U(ComponentTypes::getAssignedUILog(getHost()),
            "Send --> \"%s\" to %s: at %s",
            MessageTypes::getName(msg->getHeader()->getType()),
            ComponentTypes::getName(target),
            InterfaceTypes::getAddressString(address).c_str());

    return interfaces[target]->push(MESSAGE_SEND, address, msg);
}

bool Component::send(COMPONENT target, Message *msg) {

    LOG_U(ComponentTypes::getAssignedUILog(getHost()),
          "Send --> \"%s\" as MultiCast",
          MessageTypes::getName(msg->getHeader()->getType()));

    return interfaces[target]->push(MESSAGE_SEND, interfaces[target]->getMulticastAddress(), msg);
}

bool Component::put(COMPONENT target, long address, Message *msg) {

    return interfaces[target]->push(MESSAGE_RECEIVE, address, msg);
}

std::vector<long> Component::getAddressList(COMPONENT target) {

    return interfaces[target]->getDevice()->getAddressList();
}

