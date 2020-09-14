//
// Created by Haluk Akgunduz on 10/14/15.
//

#include "Component.h"
#include "NetUtil.h"

void *Component::notifyContext = nullptr;
TypeNotifyCB Component::notifyCB = nullptr;

TypeComponent Component::nullComponent = nullptr;

Component::Component(TypeHostUnit _host) {

    host = std::move(_host);

    receiverCB = new InterfaceSchedulerCB([](void *arg, const TypeSchedulerItem& item) -> bool {

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

    interfaces[COMP_NODE] = CommInterfaceFactory::createInterface(getHost(), nodeDevice, receiverCB);

    interfaces[COMP_DISTRIBUTOR] = otherDevice != nodeDevice && type != COMP_NODE ?
                                   CommInterfaceFactory::createInterface(getHost(), otherDevice, receiverCB) :
                                   interfaces[COMP_NODE];

    interfaces[COMP_COLLECTOR] = interfaces[COMP_DISTRIBUTOR];

    if (!interfaces[COMP_NODE] || !interfaces[COMP_DISTRIBUTOR]) {

        return false;
    }

    return true;
}

bool Component::shutdownInterfaces() {

    if (interfaces[COMP_DISTRIBUTOR] != interfaces[COMP_NODE]) {

        interfaces[COMP_DISTRIBUTOR]->shutdown();
    }

    interfaces[COMP_NODE]->shutdown();

    return false;
}

Component::~Component() {

    LOGP_T("Deallocating Component");

    sendShutdownMsg(getComponent(getHost()->getType()));

    while(true);

    delete receiverCB;
}

bool Component::onReceive(const TypeComponentUnit& owner, MSG_TYPE msgType, TypeMessage msg) {

    LOGC_D(getHost(), owner, MSGDIR_RECEIVE,
           "\"%s\" is received",
           MessageType::getMsgName(msgType));

    auto processCB = processMsg[owner->getType()].find(msgType);
    if (processCB == processMsg[owner->getType()].end()) {

        auto processStaticCB = processStaticMsg[owner->getType()].find(msgType);
        if (processStaticCB == processStaticMsg[owner->getType()].end()) {

            if (msgType == MSGTYPE_SHUTDOWN) {

                return shutdownProcessMsg(owner, std::move(msg));
            }

            return defaultProcessMsg(owner, std::move(msg));
        }

        return (processStaticMsg[owner->getType()][msgType])(shared_from_this(), owner, std::move(msg));
    }

    return (this->*processMsg[owner->getType()][msgType])(owner, std::move(msg));
}

bool Component::shutdownProcessMsg(const TypeComponentUnit& owner, TypeMessage msg) {

    LOGC_W(getHost(), owner, MSGDIR_RECEIVE, "Shutdown is received");

    shutdownInterfaces();

    return true;
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

    return interfaces[target]->getDevice();
}

Address &Component::getInterfaceAddress(COMPONENT target) {

    return interfaces[target]->getAddress();
}

Address &Component::getInterfaceMulticastAddress(COMPONENT target) {

    return interfaces[target]->getMulticastAddress();
}

TypeAddressList Component::getInterfaceAddressList(COMPONENT target) {

    return interfaces[target]->getAddressList();
}

COMM_INTERFACE Component::getInterfaceType(COMPONENT target) {

    return interfaces[target]->getType();
}

bool Component::isSupportMulticast(COMPONENT target) {

    return interfaces[target]->isSupportMulticast();
}

bool Component::send(const TypeComponentUnit& target, TypeMessage msg) {

    return interfaces[target->getType()]->push(MSGDIR_SEND, target, std::move(msg));
}

bool Component::sendShutdownMsg(const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(getHost(target->getType()), MSGTYPE_SHUTDOWN);

    return send(target, std::move(msg));
}

TypeHostUnit& Component::getHost() {

    return host;
}

TypeHostUnit Component::getHost(COMPONENT _out) {

    auto unit = std::make_unique<HostUnit>(*host);

    unit->setAddress(getInterfaceAddress(_out));

    return std::move(unit);
}

TypeComponentUnit Component::getComponent(COMPONENT _out) {

    auto unit = std::make_shared<ComponentUnit>(host.get());

    unit->setAddress(getInterfaceAddress(_out));

    return std::move(unit);
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
