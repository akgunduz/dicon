//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "Test.h"

void sendPing(Component *owner, const ComponentObject& target) {

    auto *msg = new Message(owner->getHost(), target.getType(), (MSG_TYPE)MSG_TYPE_TEST_PING);

    owner->send(target, msg);
}

bool processPingMsg(Component* component, const ComponentObject& owner, Message *msg) {

    LOGS_I(component->getHost(), "Message Ping has came from : %s, replying back", owner.getName());

    sendPing(component, owner);

    return true;
}

void testPing(Distributor* distributor, Collector* collector, Node* node) {

    MessageTypes::addMsg(MSG_TYPE_TEST_PING, "TEST_PING");

    distributor->addStaticProcessHandler(COMP_NODE, (MSG_TYPE)MSG_TYPE_TEST_PING, processPingMsg);
    node->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_PING, processPingMsg);

    ComponentObject target(node->getHost(), COMP_DISTRIBUTOR);
    sendPing(distributor, target);
}
