//
// Created by Haluk Akgunduz on 10/14/15.
//

#include "Component.h"
#include "Util.h"

void *Component::notifyContext = nullptr;
TypeNotifyCB Component::notifyCB = nullptr;

Component::Component(const char* rootPath) {

    strcpy(this->rootPath, rootPath);

    schedulerCB = new InterfaceSchedulerCB(receiveCB, this);
    hostCB = new InterfaceHostCB(getHostCB, this);
}

bool Component::initInterfaces(COMPONENT type) {

    DeviceList *deviceList = DeviceList::getInstance();

    interfaces[COMP_NODE] = Connector::createInterface(deviceList->getActive(1), schedulerCB, hostCB);
    interfaces[COMP_DISTRIBUTOR] = deviceList->isActiveDifferent() && type != COMP_NODE ?
                                   Connector::createInterface(deviceList->getActive(0), schedulerCB, hostCB) :
                                   interfaces[COMP_NODE];
    interfaces[COMP_COLLECTOR] = interfaces[COMP_DISTRIBUTOR];

    host->setAddress(ComponentInfo::next(type), getInterfaceAddress(ComponentInfo::next(type)));
    host->setAddress(ComponentInfo::prev(type), getInterfaceAddress(ComponentInfo::prev(type)));

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

const char* Component::getRootPath() {

    return rootPath;
}

ComponentObject& Component::getHostCB(void *arg) {

    return ((Component*) arg)->getHost();
}

bool Component::receiveCB(void *arg, SchedulerItem* item) {

    auto *component = (Component *) arg;
    auto *messageItem = (MessageItem*) item;

    return component->onReceive(messageItem->getMessage()->getHeader().getOwner(),
                                messageItem->getMessage()->getHeader().getType(),
                                messageItem->getMessage());
}

bool Component::onReceive(const ComponentObject& owner, MSG_TYPE msgType, Message *msg) {

    LOGC_I(getHost(),
           owner,
          false,
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


bool Component::defaultProcessMsg(const ComponentObject& owner, Message *msg) {

    delete msg;
    return true;
}


long Component::getInterfaceAddress(COMPONENT target) {

    if (interfaces[target] != nullptr) {
        return interfaces[target]->getAddress();
    }

    return 0;
}

long Component::getInterfaceMulticastAddress(COMPONENT target) {

    if (interfaces[target] != nullptr) {
        return interfaces[target]->getMulticastAddress();
    }

    return 0;
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

bool Component::send(const ComponentObject& target, Message *msg) {

    LOGC_I(getHost(),
           target,
           true,
           "\"%s\" is sent",
           MessageTypes::getMsgName(msg->getHeader().getType()));

    return interfaces[target.getType()]->push(MESSAGE_SEND, target.getAddress(target.getType()), msg);
}

std::vector<long> Component::getAddressList(const ComponentObject& target) {

    return interfaces[target.getType()]->getDevice()->getAddressList();
}

ComponentObject &Component::getHost() {
    return (*host);
}

void Component::registerNotify(void* _notifyContext, TypeNotifyCB _notifyCB) {

    notifyContext = _notifyContext;
    notifyCB = _notifyCB;
}

void Component::notifyUI() {

    if (notifyContext) {
        Timer::set(getHost().getType(), 1000, notifyCB, notifyContext, getHost().getID());
    }
}

bool Component::isIDAssigned() {

    return getHost().getID() != 0;
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
