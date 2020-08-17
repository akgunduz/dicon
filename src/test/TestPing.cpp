//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendPing(Component *owner, ComponentUnit& target) {

    auto *msg = new Message(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_PING);

    owner->send(target, msg);
}

bool processPingMsg(Component* component, ComponentUnit& owner, Message *msg) {

    LOGS_I(component->getHost(), "Message Ping has came from : %s, replying back", ComponentType::getName(owner.getType()));

    sendPing(component, owner);

    return true;
}

void TestApp::testPing(Distributor* distributor, Collector* collector, Node* node) {

    MessageTypes::addMsg(MSG_TYPE_TEST_PING, "TEST_PING");

    distributor->addStaticProcessHandler(COMP_NODE, (MSG_TYPE)MSG_TYPE_TEST_PING, processPingMsg);
    node->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_PING, processPingMsg);

    ComponentUnit target(COMP_NODE, node->getHost().getArch(), node->getHost().getID(), node->getHost().getAddress(COMP_DISTRIBUTOR));
    sendPing(distributor, target);
}
