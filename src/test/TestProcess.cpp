//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendProcess(const TypeComponent& owner, ComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_PROCESS);

    auto job = std::make_unique<JobItem>(owner->getHost(), "../sample/Job1_x86_linux.zip", JobItem::jobID++);

    job->getProcess(0)->check();

    msg->getData().setStreamFlag(STREAM_PROCESS);
    msg->getData().addProcess(job->getProcess(0));

    owner->send(target, std::move(msg));
}

bool processProcessMsg(const TypeComponent& component, ComponentUnit& owner, TypeMessage msg) {

    auto processItem = msg->getData().getProcess(0);

    LOGS_I(component->getHost(), "Message Process[%d] has came from : %s",
           processItem->getID(),
           ComponentType::getName(owner.getType()).c_str());

    return true;
}

void TestApp::testProcess(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_PROCESS, "TEST_PROCESS");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_PROCESS, processProcessMsg);

    ComponentUnit target(COMP_NODE, node->getHost()->getArch(), node->getHost()->getID(),
                         node->getHost()->getAddress(COMP_COLLECTOR));
    sendProcess((TypeComponent&) collector, target);
}
