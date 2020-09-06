//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendWakeUp(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_WAKEUP);

    owner->send(target, std::move(msg));
}

void sendAlive(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_ALIVE);

    owner->send(target, std::move(msg));
}

bool processWakeUpMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    LOGC_I(component->getHost(), owner, MSGDIR_SEND, "Sending Alive");

    sendAlive(component, owner);
    return true;
}

bool processAliveMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    return true;
}

void TestApp::testSendWakeUp(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_WAKEUP, "TEST_WAKEUP");
    MessageType::addMsg(MSG_TYPE_TEST_ALIVE, "TEST_ALIVE");

    collector->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_WAKEUP, processWakeUpMsg);
    node->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_WAKEUP, processWakeUpMsg);

    distributor->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_ALIVE, processAliveMsg);
    distributor->addStaticProcessHandler(COMP_NODE, (MSG_TYPE)MSG_TYPE_TEST_ALIVE, processAliveMsg);

    auto target = std::make_shared<ComponentUnit>(COMP_NODE, distributor->getInterfaceMulticastAddress(COMP_NODE));
    target->getAddress().setMulticast(true);

    sendWakeUp((TypeComponent &) distributor, target);
}
