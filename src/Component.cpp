//
// Created by Haluk Akgunduz on 10/14/15.
//

#include "Component.h"
#include "CommInterfaceFactory.h"
#include "CommInterfaceCB.h"
#include "DeviceList.h"
#include "MessageItem.h"
#include "Log.h"

Component::Component(TypeHostUnit _host)
    : host(std::move(_host)) {

    receiverCB = new CommInterfaceCB([](void *arg, const TypeSchedulerItem& item) -> bool {

        auto *component = (Component *) arg;

        auto msgItem = std::static_pointer_cast<MessageItem>(item);

        auto owner = std::make_shared<ComponentUnit>(msgItem->getMessage()->getHeader().getOwner());

        auto msgType = msgItem->getMessage()->getHeader().getType();

        return component->onReceive(owner, msgType, std::move(msgItem->getMessage()));

    }, this);
}

bool Component::initInterfaces(COMPONENT type, int interfaceOther, int interfaceNode) {

    auto *deviceList = DeviceList::getInstance();

    auto &nodeDevice = deviceList->get(interfaceNode);
    auto &otherDevice = deviceList->get(interfaceOther);

    interfaceList[nodeDevice->getType()] = CommInterfaceFactory::createInterface(getHost(), nodeDevice, receiverCB);
    interfaceMap[COMP_NODE] = interfaceList[nodeDevice->getType()];
    host->setIFaceAddress(nodeDevice->getType(), interfaceList[nodeDevice->getType()]->getAddress());
    host->setAddress(COMP_NODE, interfaceMap[COMP_NODE]->getAddress());

    if (otherDevice != nodeDevice && type != COMP_NODE) {
        interfaceList[otherDevice->getType()] = CommInterfaceFactory::createInterface(getHost(), otherDevice, receiverCB);
        interfaceMap[COMP_DISTRIBUTOR] = interfaceList[otherDevice->getType()];
        host->setIFaceAddress(otherDevice->getType(), interfaceList[otherDevice->getType()]->getAddress());

    } else {

        interfaceMap[COMP_DISTRIBUTOR] = interfaceMap[COMP_NODE];
    }

    host->setAddress(COMP_DISTRIBUTOR, interfaceMap[COMP_DISTRIBUTOR]->getAddress());

    interfaceMap[COMP_COLLECTOR] = interfaceMap[COMP_DISTRIBUTOR];
    host->setAddress(COMP_COLLECTOR, interfaceMap[COMP_COLLECTOR]->getAddress());

    return true;
}

Component::~Component() {

    LOGP_T("Deallocating Component");

    for (auto& _interface : interfaceList) {

        sendShutdownMsg(getHost()->forkComponent(_interface.first));
        _interface.second->waitThread();

        delete _interface.second;
    }

    delete receiverCB;
}

bool Component::onReceive(const TypeComponentUnit& owner, MSG_TYPE msgType, TypeMessage msg) {

    LOGC_T(getHost(), owner, MSGDIR_RECEIVE,
           "\"%s\" is received",
           MessageType::getMsgName(msgType));

    auto processCB = processMsg[owner->getType()].find(msgType);
    if (processCB == processMsg[owner->getType()].end()) {

        auto processStaticCB = processStaticMsg[owner->getType()].find(msgType);
        if (processStaticCB == processStaticMsg[owner->getType()].end()) {

            return defaultProcessMsg(owner, std::move(msg));
        }

        return (processStaticMsg[owner->getType()][msgType])(shared_from_this(), owner, std::move(msg));
    }

    return (this->*processMsg[owner->getType()][msgType])(owner, std::move(msg));
}

bool Component::defaultProcessMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    if (getHost()->getType() != owner->getType()) {
        LOGC_W(getHost(), owner, MSGDIR_RECEIVE,
               "No Handler is found for message : \"%s\"",
               MessageType::getMsgName(msg->getHeader().getType()));
    }

    return true;
}

const TypeDevice& Component::getDevice(COMPONENT target) {

    return interfaceMap[target]->getDevice();
}

TypeAddress& Component::getInterfaceAddress(COMPONENT target) {

    return interfaceMap[target]->getAddress();
}

TypeAddress& Component::getInterfaceMulticastAddress(COMPONENT target) {

    return interfaceMap[target]->getMulticastAddress();
}

TypeAddressList Component::getInterfaceAddressList(COMPONENT target) {

    return interfaceMap[target]->getAddressList();
}

COMM_INTERFACE Component::getInterfaceType(COMPONENT target) {

    return interfaceMap[target]->getType();
}

bool Component::isSupportMulticast(COMPONENT target) {

    return interfaceMap[target]->isSupportMulticast();
}

bool Component::send(const TypeComponentUnit& target, TypeMessage msg) {

    return interfaceMap[target->getType()]->push(MSGDIR_SEND, target, std::move(msg));
}

bool Component::sendShutdownMsg(const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(), target->getType(), MSGTYPE_SHUTDOWN);

    return interfaceList[target->getAddress()->getInterface()]->push(MSGDIR_SEND, target, std::move(msg));
}

TypeHostUnit& Component::getHost() {

    return host;
}

void Component::registerNotify(void *_notifyContext, TypeNotifyCB _notifyCB) {

    notifyContext = _notifyContext;
    notifyCB = _notifyCB;
}

void Component::deRegisterNotify() {

    notifyContext = nullptr;
}

bool Component::notifyUI(NOTIFYTYPE state) {

    if (notifyContext) {

        return notifyCB(notifyContext, getHost()->getType(), state);
    }

    return false;
}

bool Component::setID(TypeID& id) {

    if (id == 0) {
        LOGS_E(getHost(), "Can not assign ID with 0!!!");
        return false;
    }

    getHost()->setID(id);

    return true;
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

bool Component::isInitialized() {

    return initialized;
}
