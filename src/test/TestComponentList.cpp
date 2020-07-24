//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "Test.h"

void sendComponentList(Component *owner, const ComponentObject& target) {

    auto *msg = new Message(owner->getHost(), target.getType(), (MSG_TYPE)MSG_TYPE_TEST_COMPLIST);

    std::vector<ComponentObject> nodes;

    nodes.emplace_back(NodeObject(1, 999));
    nodes.emplace_back(NodeObject(2, 9999));

    msg->getData().setStreamFlag(STREAM_COMPONENT);
    msg->getData().setComponentList(nodes);

    owner->send(target, msg);
}

bool processComponentListMsg(Component* component, const ComponentObject& owner, Message *msg) {

    std::vector<ComponentObject> nodes = msg->getData().getComponentList();

    LOGS_I(component->getHost(), "Message Component List has came from : %s, data amount: %d", owner.getName(), nodes.size());
    LOGS_I(component->getHost(), "Node 1 : %d, %d", nodes[0].getID(), nodes[0].getAddress());
    LOGS_I(component->getHost(), "Node 2 : %d, %d", nodes[1].getID(), nodes[1].getAddress());

    return true;
}

void testComponentList(Distributor* distributor, Collector* collector, Node* node) {

    MessageTypes::addMsg(MSG_TYPE_TEST_COMPLIST, "TEST_COMPLIST");

    collector->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_COMPLIST, processComponentListMsg);

    ComponentObject target(collector->getHost(), COMP_DISTRIBUTOR);
    sendComponentList(distributor, target);
}
