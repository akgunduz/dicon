//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include <ComponentUnitFactory.h>
#include "TestApp.h"

void sendComponentList(Component *owner, ComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_COMPLIST);

    TypeComponentList nodes;
    Address address({999, 1}, {1999, 2});
    nodes.emplace_back(ComponentUnitFactory::create(COMP_NODE, ArchTypes::get(), 1, address));
    Address address2({9999, 3}, {19999, 4});
    nodes.emplace_back(ComponentUnitFactory::create(COMP_NODE, ArchTypes::get(), 2, address2));

    msg->getData().setStreamFlag(STREAM_COMPONENT);
    msg->getData().setComponentList(nodes);

    owner->send(target, std::move(msg));
}

bool processComponentListMsg(Component* component, ComponentUnit& owner, TypeMessage msg) {

    TypeComponentList nodes = msg->getData().getComponentList();

    LOGS_I(component->getHost(), "Message Component List has came from : %s, data amount: %d",
           ComponentType::getName(owner.getType()), nodes.size());
    LOGS_I(component->getHost(), "Node 1 : %d, %u", nodes[0]->getID(), nodes[0]->getAddress().get().base);
    LOGS_I(component->getHost(), "Node 2 : %d, %u", nodes[1]->getID(), nodes[1]->getAddress().get().base);

    return true;
}

void TestApp::testComponentList(Distributor* distributor, Collector* collector, Node* node) {

    MessageTypes::addMsg(MSG_TYPE_TEST_COMPLIST, "TEST_COMPLIST");

    collector->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_COMPLIST, processComponentListMsg);

    ComponentUnit target(COMP_COLLECTOR, collector->getHost().getArch(), collector->getHost().getID(),
                         collector->getHost().getAddress(COMP_DISTRIBUTOR));
    sendComponentList(distributor, target);
}
