//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendProcessID(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(target->getType()), (MSG_TYPE)MSG_TYPE_TEST_PROCESS_ID, STREAM_PROCESS_ID);

    auto job = std::make_shared<JobItem>(owner->getHost(), TEST_JOB_PATH, JobItem::jobID++);

    job->getProcess(0)->check();

    msg->getData().setProcess(job->getProcess(0));

    owner->send(target, std::move(msg));
}

bool processProcessIDMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    auto processItem = msg->getData().getProcess();

    LOGS_I(component->getHost(), "Message Process[%d] has came from : %s",
           processItem->getID(),
           ComponentType::getName(owner->getType()));

    return true;
}

void TestApp::testSendProcessID(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_PROCESS_ID, "TEST_PROCESS_ID");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_PROCESS_ID, processProcessIDMsg);

    auto target = std::make_shared<ComponentUnit>(COMP_NODE, node->getHost()->getArch(), node->getHost()->getID(),
                                                  node->getHost(COMP_COLLECTOR)->getAddress());
    sendProcessID((TypeComponent&) collector, target);
}
