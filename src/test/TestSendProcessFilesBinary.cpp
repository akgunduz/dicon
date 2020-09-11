//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendProcessFilesBinary(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(target->getType()), (MSG_TYPE)MSG_TYPE_TEST_PROCESS_FILES_BINARY, STREAM_PROCESS_FILES_BINARY);

    auto job = std::make_shared<JobItem>(owner->getHost(), TEST_JOB_ZIP, JobItem::jobID++);

    job->getProcess(0)->check();

    msg->getData().setProcess(job->getProcess(0));

    owner->send(target, std::move(msg));
}

bool processProcessFilesBinaryMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    auto list = msg->getData().getProcess()->getFileList();

    LOGS_I(component->getHost(), "Message File Binary has came from : %s with assigned Process : %d",
           ComponentType::getName(owner->getType()), msg->getData().getProcess()->getID());
    LOGS_I(component->getHost(), "File 1 : %d, %s, %d", list[0]->get()->getID(), list[0]->get()->getName().c_str(), list[0]->get()->getSize());
    LOGS_I(component->getHost(), "File 2 : %d, %s, %d", list[1]->get()->getID(), list[1]->get()->getName().c_str(), list[1]->get()->getSize());
    LOGS_I(component->getHost(), "File 3 : %d, %s, %d", list[2]->get()->getID(), list[2]->get()->getName().c_str(), list[2]->get()->getSize());
    LOGS_I(component->getHost(), "File 4 : %d, %s, %d", list[3]->get()->getID(), list[3]->get()->getName().c_str(), list[3]->get()->getSize());

    return true;
}

void TestApp::testSendProcessFilesBinary(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_PROCESS_FILES_BINARY, "TEST_PROCESS_FILES_BINARY");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_PROCESS_FILES_BINARY, processProcessFilesBinaryMsg);

    auto target = std::make_shared<ComponentUnit>(COMP_NODE, node->getHost()->getArch(), node->getHost()->getID(),
                                                  node->getHost(COMP_COLLECTOR)->getAddress());
    sendProcessFilesBinary((TypeComponent&) collector, target);
}
