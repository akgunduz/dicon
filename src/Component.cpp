//
// Created by Haluk Akgunduz on 10/14/15.
//

#include "Component.h"

Component::Component(COMPONENT host, const char* rootPath) :
    host(host) {

    strcpy(this->rootPath, rootPath);
    this->host.setRootPath(getRootPath());

    schedulerCB = new InterfaceSchedulerCB(receiveCB, this);
    hostCB = new InterfaceHostCB(getHostCB, this);

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

const char* Component::getRootPath() {

    return rootPath;
}

ComponentObject Component::getHost() {

    return host;
}

void Component::setHostID(int id) {

    host.setID(id);
}

ComponentObject Component::getHostCB(void *arg) {

    return ((Component*) arg)->getHost();
}

bool Component::receiveCB(void *arg, SchedulerItem* item) {

    auto *component = (Component *) arg;
    auto *messageItem = (MessageItem*) item;

    return component->onReceive(messageItem->getMessage()->getHeader()->getOwner(),
                                messageItem->getMessage()->getHeader()->getType(),
                                messageItem->getMessage());
}

bool Component::onReceive(ComponentObject owner, MSG_TYPE msgType, Message *msg) {

    LOGC_I(getHost(),
           owner,
          false,
          "\"%s\" is received",
          MessageTypes::getName(msgType));

    auto processCB = processMsg[owner.getType()].find(msgType);
    if (processCB == processMsg[owner.getType()].end()) {

        return defaultProcessMsg(owner, msg);
    }

    return (this->*processMsg[owner.getType()][msgType])(owner, msg);
}


bool Component::defaultProcessMsg(ComponentObject owner, Message *msg) {

    delete msg;
    return true;
}


long Component::getInterfaceAddress(ComponentObject target) {

    if (interfaces[target.getType()] != NULL) {
        return interfaces[target.getType()]->getAddress();
    }

    return 0;
}

long Component::getInterfaceMulticastAddress(ComponentObject target) {

    if (interfaces[target.getType()] != NULL) {
        return interfaces[target.getType()]->getMulticastAddress();
    }

    return 0;
}

INTERFACE Component::getInterfaceType(ComponentObject target) {

    if (interfaces[target.getType()] != NULL) {
        return interfaces[target.getType()]->getType();
    }

    return INTERFACE_MAX;
}


bool Component::isSupportMulticast(ComponentObject target) {

    if (interfaces[target.getType()] != NULL) {
        return interfaces[target.getType()]->isSupportMulticast();
    }

    return false;
}

bool Component::send(ComponentObject target, Message *msg) {

    LOGC_I(getHost(),
           target,
           true,
           "\"%s\" is sent",
           MessageTypes::getName(msg->getHeader()->getType()));

    ComponentObject object(getHost().getType(), getHost().getRootPath(),
                           getHost().getID(), interfaces[target.getType()]->getAddress());

    msg->getHeader()->setOwner(object);

    return interfaces[target.getType()]->push(MESSAGE_SEND, target.getAddress(), msg);
}

std::vector<long> Component::getAddressList(ComponentObject target) {

    return interfaces[target.getType()]->getDevice()->getAddressList();
}

