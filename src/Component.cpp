//
// Created by Haluk Akgunduz on 10/14/15.
//

#include "Component.h"

Component::Component(Unit host, const char* rootPath) {

    memset(connectors, 0, sizeof(connectors));

    setHost(host);
    Unit::setRootPath(getHost().getType(), rootPath);

    callback = new InterfaceCallback(receiveCB, this);

    switch(host.getType()) {

        case COMP_DISTRIBUTOR:
            connectors[COMP_COLLECTOR] = new Connector(host, Connector::getSelectedDevice(0), callback);
            if (isConnectorDifferent()) {
                connectors[COMP_NODE] = new Connector(host, Connector::getSelectedDevice(1), callback);
            } else {
                connectors[COMP_NODE] = connectors[COMP_COLLECTOR];

            }
            break;

        case COMP_COLLECTOR:
            connectors[COMP_DISTRIBUTOR] = new Connector(host, Connector::getSelectedDevice(0), callback);
            if (isConnectorDifferent()) {
                connectors[COMP_NODE] = new Connector(host, Connector::getSelectedDevice(1), callback);
            } else {
                connectors[COMP_NODE] = connectors[COMP_DISTRIBUTOR];
            }
            break;

        case COMP_NODE:
            connectors[COMP_DISTRIBUTOR] = new Connector(host, Connector::getSelectedDevice(1), callback);
            connectors[COMP_COLLECTOR] = connectors[COMP_DISTRIBUTOR];
            break;

        default:
            break;
    }
}

Component::~Component() {

    if (connectors[COMP_NODE] != connectors[COMP_DISTRIBUTOR] &&
            connectors[COMP_NODE] != connectors[COMP_COLLECTOR]) {
        delete connectors[COMP_NODE];
    }

    if (connectors[COMP_COLLECTOR] != connectors[COMP_DISTRIBUTOR]) {
        delete connectors[COMP_COLLECTOR];
    }

    delete connectors[COMP_DISTRIBUTOR];

    delete callback;
}

bool Component::isConnectorDifferent() {

    return Connector::getSelectedDevice(0) != Connector::getSelectedDevice(1);
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

    if (connectors[msg->getHeader()->getOwner().getType()] == nullptr ||
            connectors[msg->getHeader()->getOwner().getType()]->getInterfaceType() != Address::getInterface(address)) {
        LOG_W("%s: Wrong message received : %s from %s, disgarding", getHost().getTypeName(),
              MessageTypes::getName(msg->getHeader()->getType()), Address::getString(address).c_str());
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

long Component::getAddress(COMPONENT host) {

    if (connectors[host] != NULL) {
        return connectors[host]->getAddress();
    }

    return 0;
}

Unit Component::getHost() {

    return host;
}

bool Component::send(COMPONENT target, long address, Message *msg) {

    LOG_U(ComponentTypes::getAssignedUILog(getHost().getType()),
            "Send : \"%s\" to %s: at %s",
            MessageTypes::getName(msg->getHeader()->getType()),
            ComponentTypes::getName(target),
            Address::getString(address).c_str());

    return connectors[target]->send(address, msg);
}

bool Component::send(COMPONENT target, Message *msg) {

    LOG_U(ComponentTypes::getAssignedUILog(getHost().getType()),
          "Send : \"%s\" as MultiCast",
          MessageTypes::getName(msg->getHeader()->getType()));

    return connectors[target]->send(msg);
}

bool Component::put(COMPONENT target, long address, Message *msg) {

    return connectors[target]->put(address, msg);
}


std::vector<long> Component::getAddressList(COMPONENT target) {

    return connectors[target]->getAddressList();
}
