//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

bool sendWakeup(const TypeComponent& owner, COMPONENT targetType) {

    if (owner->isSupportMulticast(targetType)) {

        auto target = std::make_shared<ComponentUnit>(targetType, owner->getInterfaceMulticastAddress(targetType));

        auto msg = std::make_unique<Message>(owner->getHost(), target->getType(), (MSG_TYPE)MSG_TYPE_TEST_WAKEUP);

        owner->send(target, std::move(msg));

    } else {

        TypeAddressList list = owner->getInterfaceAddressList(targetType);

        for (auto &address : list) {

            auto target = std::make_shared<ComponentUnit>(targetType, address);

            auto msg = std::make_unique<Message>(owner->getHost(), target->getType(), (MSG_TYPE)MSG_TYPE_TEST_WAKEUP);

            owner->send(target, std::move(msg));
        }
    }

    return true;
}

bool sendWakeupMessagesAll(const TypeComponent& owner) {

    LOGS_I(owner->getHost(), "Sending Wakeup");

    sendWakeup(owner, COMP_NODE);

    if (owner->getDevice(COMP_NODE) != owner->getDevice(COMP_COLLECTOR)) {
        sendWakeup(owner, COMP_COLLECTOR);
    }

    return true;
}

void sendAlive(const TypeComponent& owner, const TypeComponentUnit& target) {

    LOGC_I(owner->getHost(), target, MSGDIR_SEND, "Sending Alive");

    auto msg = std::make_unique<Message>(owner->getHost(), target->getType(), (MSG_TYPE)MSG_TYPE_TEST_ALIVE);

    owner->send(target, std::move(msg));
}

bool processWakeUpMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    LOGC_I(component->getHost(), owner, MSGDIR_RECEIVE, "Wakeup is Received");

    sendAlive(component, owner);
    return true;
}

bool processAliveMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    LOGC_I(component->getHost(), owner, MSGDIR_RECEIVE, "Alive is Received");
    return true;
}

void TestApp::testSendWakeUp(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_WAKEUP, "TEST_WAKEUP");
    MessageType::addMsg(MSG_TYPE_TEST_ALIVE, "TEST_ALIVE");

    collector->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_WAKEUP, processWakeUpMsg);
    node->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_WAKEUP, processWakeUpMsg);

    distributor->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_ALIVE, processAliveMsg);
    distributor->addStaticProcessHandler(COMP_NODE, (MSG_TYPE)MSG_TYPE_TEST_ALIVE, processAliveMsg);

  //  auto target = std::make_shared<ComponentUnit>(COMP_NODE, distributor->getInterfaceMulticastAddress(COMP_NODE));

    sendWakeupMessagesAll(distributor);
}
