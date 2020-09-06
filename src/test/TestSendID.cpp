//
// Created by akgun on 5.09.2020.
//

#include "TestApp.h"

bool send2DistributorIDMsg(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_ID);

    return owner->send(target, std::move(msg));
}

bool sendIDMsg(const TypeComponent& owner, const TypeComponentUnit& target, TypeID id) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_ID, STREAM_ID);

    msg->getData().setID(id);

    return owner->send(target, std::move(msg));
}

bool processOtherIDMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    LOGC_I(component->getHost(), owner, MSGDIR_RECEIVE, "%s[%d] is confirmed",
           ComponentType::getName(owner->getType()), owner->getID());

    return true;
}

bool processDistIDMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    if (!component->setID(msg->getData().getID())) {

        LOGC_E(component->getHost(), owner, MSGDIR_RECEIVE, "New ID : 0 is tried to assigned, it should not!!!");

        return false;
    }

    LOGC_I(component->getHost(), owner, MSGDIR_RECEIVE, "New ID : %d is assigned by Distributor", component->getHost()->getID());

    return send2DistributorIDMsg(component, owner);
}

void TestApp::testSendID(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_ID, "TEST_ID");

    collector->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_ID, processDistIDMsg);
    node->addStaticProcessHandler(COMP_DISTRIBUTOR, (MSG_TYPE)MSG_TYPE_TEST_ID, processDistIDMsg);

    distributor->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_ID, processOtherIDMsg);
    distributor->addStaticProcessHandler(COMP_NODE, (MSG_TYPE)MSG_TYPE_TEST_ID, processOtherIDMsg);

    auto collTarget = std::make_shared<ComponentUnit>(COMP_COLLECTOR,
                                                      collector->getHost()->getArch(),
                                                      collector->getHost()->getID(),
                                                      collector->getHost()->getAddress(COMP_DISTRIBUTOR));

    auto nodeTarget = std::make_shared<ComponentUnit>(COMP_NODE,
                                                      node->getHost()->getArch(),
                                                      node->getHost()->getID(),
                                                      node->getHost()->getAddress(COMP_DISTRIBUTOR));

//    for (int i = 1; i < 1000; i++) {
//        sendIDMsg((TypeComponent &) distributor, collTarget, i);
//        sendIDMsg((TypeComponent &) distributor, nodeTarget, i);
//    }

    sendIDMsg((TypeComponent &) distributor, collTarget, 1);
}
