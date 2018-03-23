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
            if (Connector::getSelectedDevice(0) != Connector::getSelectedDevice(1)) {
                connectors[COMP_NODE] = new Connector(host, Connector::getSelectedDevice(1), callback);
            } else {
                connectors[COMP_NODE] = connectors[COMP_COLLECTOR];
            }
            break;
        case COMP_COLLECTOR:
            connectors[COMP_DISTRIBUTOR] = new Connector(host, Connector::getSelectedDevice(0), callback);
            if (Connector::getSelectedDevice(0) != Connector::getSelectedDevice(1)) {
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
        LOG_W("%s: Wrong message received : %d from %s, disgarding", getHost().getTypeName(),
              msg->getHeader()->getType(), Address::getString(address).c_str());
        delete msg;
        return false;
    }

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

    if (connectors[host] != nullptr) {
        return connectors[host]->getAddress();
    }

    return 0;
}

Unit Component::getHost() {

    return host;
}
