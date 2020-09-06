//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendProcess(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_PROCESS, STREAM_PROCESS);

    auto job = std::make_shared<JobItem>(owner->getHost(), "../sample/Job1_x86_linux.zip", JobItem::jobID++);

    job->getProcess(0)->check();

    msg->getData().setProcess(job->getProcess(0));

    owner->send(target, std::move(msg));
}

bool processProcessMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    auto processItem = msg->getData().getProcess();

    LOGS_I(component->getHost(), "Message Process[%d] has came from : %s",
           processItem->getID(),
           ComponentType::getName(owner->getType()));

    return true;
}

void TestApp::testProcess(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_PROCESS, "TEST_PROCESS");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_PROCESS, processProcessMsg);

    auto target = std::make_shared<ComponentUnit>(COMP_NODE, node->getHost()->getArch(), node->getHost()->getID(),
                         node->getHost()->getAddress(COMP_COLLECTOR));
    sendProcess((TypeComponent&) collector, target);
}
