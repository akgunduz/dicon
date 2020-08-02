//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendWakeUp(Component *owner, ComponentUnit& target) {

    auto *msg = new Message(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_WAKEUP);

    owner->send(target, msg);
}

void sendAlive(Component *owner, ComponentUnit& target) {

    auto *msg = new Message(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_ALIVE);

    owner->send(target, msg);
}

bool processWakeUpMsg(Component* component, ComponentUnit& owner, Message *msg) {

    LOGC_I(component->getHost(), owner, MSGDIR_SEND, "Sending Alive");

    sendAlive(component, owner);
    return true;
}

bool processAliveMsg(Component* component, ComponentUnit& owner, Message *msg) {

    return true;
}

void TestApp::testWakeUp(Distributor* distributor, Collector* collector, Node* node) {

    MessageTypes::addMsg(MSG_TYPE_TEST_WAKEUP, "TEST_WAKEUP");
    MessageTypes::addMsg(MSG_TYPE_TEST_ALIVE, "TEST_ALIVE");

    collector->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_WAKEUP, processWakeUpMsg);
    node->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_WAKEUP, processWakeUpMsg);

    distributor->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_ALIVE, processAliveMsg);
    distributor->addStaticProcessHandler(COMP_NODE, (MSG_TYPE)MSG_TYPE_TEST_ALIVE, processAliveMsg);

    ComponentUnit target(COMP_NODE);
    target.setAddress(distributor->getInterfaceMulticastAddress(target.getType()), true);
    sendWakeUp(distributor, target);
}
