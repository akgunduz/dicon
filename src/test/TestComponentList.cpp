//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include <ComponentUnitFactory.h>
#include "TestApp.h"

void sendComponentList(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_COMPONENTS, STREAM_COMPONENTS);

    TypeComponentUnitList nodes;
    Address address({999, 1}, {1999, 2});
    nodes.emplace_back(ComponentUnitFactory::create(COMP_NODE, ArchType::get(), 1, address));
    Address address2({9999, 3}, {19999, 4});
    nodes.emplace_back(ComponentUnitFactory::create(COMP_NODE, ArchType::get(), 2, address2));

    msg->getData().setComponentList(nodes);

    owner->send(target, std::move(msg));
}

bool processComponentListMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    TypeComponentUnitList nodes = msg->getData().getComponentList();

    LOGS_I(component->getHost(), "Message Component List has came from : %s, data amount: %d",
           ComponentType::getName(owner->getType()), nodes.size());
    LOGS_I(component->getHost(), "Node 1 : %d, %u", nodes[0]->getID(), nodes[0]->getAddress().get().base);
    LOGS_I(component->getHost(), "Node 2 : %d, %u", nodes[1]->getID(), nodes[1]->getAddress().get().base);

    return true;
}

void TestApp::testComponentList(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_COMPONENTS, "TEST_COMPLIST");

    collector->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_COMPONENTS, processComponentListMsg);

    auto target = std::make_shared<ComponentUnit>(COMP_COLLECTOR, collector->getHost()->getArch(), collector->getHost()->getID(),
                         collector->getHost()->getAddress(COMP_DISTRIBUTOR));
    sendComponentList((TypeComponent &) distributor, target);
}
