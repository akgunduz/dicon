//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendProcessInfo(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target->getType(), (MSG_TYPE)MSG_TYPE_TEST_PROCESS_INFO, STREAM_PROCESS_INFO);

    auto job = std::make_shared<JobItem>(owner->getHost(), TEST_JOB_PATH, JobItem::jobID++);

    job->getProcess(0)->checkContent();

    msg->getData().setProcess(job->getProcess(0));

    owner->send(target, std::move(msg));
}

bool processProcessInfoMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    auto processItem = msg->getData().getProcess();

    LOGS_I(component->getHost(), "Message Process[%d] has came from : %s",
           processItem->getID(),
           ComponentType::getName(owner->getType()));

    return true;
}

void TestApp::testSendProcessInfo(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_PROCESS_INFO, "TEST_PROCESS_INFO");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_PROCESS_INFO, processProcessInfoMsg);

    auto target = std::make_shared<ComponentUnit>(COMP_NODE, node->getHost()->getArch(), node->getHost()->getID(),
                                                  node->getHost()->getAddress(COMP_COLLECTOR));
    sendProcessInfo((TypeComponent&) collector, target);
}
