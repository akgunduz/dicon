//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendWakeUp(Component *owner, const ComponentObject& target) {

    auto *msg = new Message(owner->getHost(), target.getType(), (MSG_TYPE)MSG_TYPE_TEST_WAKEUP);

    owner->send(target, msg);
}

bool processWakeUpMsg(Component* component, const ComponentObject& owner, Message *msg) {

    LOGS_I(component->getHost(), "WakeUp Message has came from : %s", owner.getName());

    return true;
}

void TestApp::testWakeUp(Distributor* distributor, Collector* collector, Node* node) {

    MessageTypes::addMsg(MSG_TYPE_TEST_WAKEUP, "TEST_WAKEUP");

    collector->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_WAKEUP, processWakeUpMsg);
    node->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_WAKEUP, processWakeUpMsg);

    ComponentObject target(ComponentObject(COMP_COLLECTOR), COMP_DISTRIBUTOR);
    target.setAddress(distributor->getInterfaceMulticastAddress(target.getType()));
    sendWakeUp(distributor, target);
}
