//
// Created by Haluk Akgunduz on 10/14/15.
//

#include "Component.h"

Component::Component(Unit host, const char* rootPath) {

    memset(interfaces, 0, sizeof(Interface));

    setHost(host);
    Unit::setRootPath(getHost().getType(), rootPath);

    callback = new InterfaceCallback(receiveCB, this);

    switch(host.getType()) {

        case COMP_DISTRIBUTOR:
            interfaces[COMP_COLLECTOR] = Connector::createInterface(host, Connector::getSelectedDevice(0), callback);
            if (isInterfaceDifferent()) {
                interfaces[COMP_NODE] = Connector::createInterface(host, Connector::getSelectedDevice(1), callback);
            } else {
                interfaces[COMP_NODE] = interfaces[COMP_COLLECTOR];

            }
            break;

        case COMP_COLLECTOR:
            interfaces[COMP_DISTRIBUTOR] = Connector::createInterface(host, Connector::getSelectedDevice(0), callback);
            if (isInterfaceDifferent()) {
                interfaces[COMP_NODE] = Connector::createInterface(host, Connector::getSelectedDevice(1), callback);
            } else {
                interfaces[COMP_NODE] = interfaces[COMP_DISTRIBUTOR];
            }
            break;

        case COMP_NODE:
            interfaces[COMP_DISTRIBUTOR] = Connector::createInterface(host, Connector::getSelectedDevice(1), callback);
            interfaces[COMP_COLLECTOR] = interfaces[COMP_DISTRIBUTOR];
            break;

        default:
            break;
    }
}

Component::~Component() {

    if (interfaces[COMP_NODE] != interfaces[COMP_DISTRIBUTOR] &&
            interfaces[COMP_NODE] != interfaces[COMP_COLLECTOR]) {
        delete interfaces[COMP_NODE];
    }

    if (interfaces[COMP_COLLECTOR] != interfaces[COMP_DISTRIBUTOR]) {
        delete interfaces[COMP_COLLECTOR];
    }

    delete interfaces[COMP_DISTRIBUTOR];

    delete callback;
}

bool Component::isInterfaceDifferent() {

    return Connector::getSelectedDevice(0) != Connector::getSelectedDevice(1);
}

Unit Component::getHost() {

    return host;
}

void Component::setHost(Unit host) {

    this->host = host;
}

bool Component::receiveCB(void *arg, SchedulerItem* item) {

    Component *component = (Component *) arg;
    MessageItem *messageItem = (MessageItem*) item;

    return component->onReceive(messageItem->address, messageItem->msg);
}

bool Component::onReceive(long address, Message *msg) {

    if (msg->getHeader()->getOwner().getType() == getHost().getType()) {
        LOG_W("%s: Loopback message %s received, ignoring ", getHost().getTypeName(),
              MessageTypes::getName(msg->getHeader()->getType()));
        delete msg;
        return false;
    }

    LOG_U(ComponentTypes::getAssignedUILog(getHost().getType()),
          "Receive : \"%s\" from %s: at %s",
          MessageTypes::getName(msg->getHeader()->getType()),
          ComponentTypes::getName(msg->getHeader()->getOwner().getType()),
          Address::getString(address).c_str());

    switch(msg->getHeader()->getOwner().getType()) {

        case COMP_DISTRIBUTOR:
            return processDistributorMsg(address, msg);

        case COMP_NODE:
            return processNodeMsg(address, msg);

        case COMP_COLLECTOR:
            return processCollectorMsg(address, msg);

        default:
            LOG_W("Wrong message received : %d from %s, disgarding",
                  msg->getHeader()->getType(), Address::getString(address).c_str());
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


INTERFACES Component::getInterfaceType(COMPONENT target) {

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

    LOG_U(ComponentTypes::getAssignedUILog(getHost().getType()),
            "Send : \"%s\" to %s: at %s",
            MessageTypes::getName(msg->getHeader()->getType()),
            ComponentTypes::getName(target),
            Address::getString(address).c_str());

    return interfaces[target]->push(MESSAGE_SEND, address, msg);
}

bool Component::send(COMPONENT target, Message *msg) {

    LOG_U(ComponentTypes::getAssignedUILog(getHost().getType()),
          "Send : \"%s\" as MultiCast",
          MessageTypes::getName(msg->getHeader()->getType()));

    return interfaces[target]->push(MESSAGE_SEND, interfaces[target]->getMulticastAddress(), msg);
}

bool Component::put(COMPONENT target, long address, Message *msg) {

    return interfaces[target]->push(MESSAGE_RECEIVE, address, msg);
}


std::vector<long> Component::getAddressList(COMPONENT target) {

    return interfaces[target]->getAddressList();
}

