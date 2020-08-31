//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendProcessFiles(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_FILEBINARY, STREAM_PROCESSFILEBINARY_ALL);

    auto job = std::make_shared<JobItem>(owner->getHost(), TEST_JOB_ZIP, JobItem::jobID++);

 //   auto list = job->getProcess(0)->getFileList();

    job->getProcess(0)->check();

   // msg->getData().setProcess(job->getProcess(0)->getID(), list);
    msg->getData().setProcess(job->getProcess(0));

    owner->send(target, std::move(msg));
}

bool processProcessFilesMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    auto list = msg->getData().getProcess()->getFileList();

    LOGS_I(component->getHost(), "Message File Binary has came from : %s with assigned Process : %d",
           ComponentType::getName(owner->getType()), msg->getData().getProcess()->getID());
    LOGS_I(component->getHost(), "File 1 : %d, %s, %d", list[0]->get()->getID(), list[0]->get()->getName().c_str(), list[0]->get()->getSize());
    LOGS_I(component->getHost(), "File 2 : %d, %s, %d", list[1]->get()->getID(), list[1]->get()->getName().c_str(), list[1]->get()->getSize());
    LOGS_I(component->getHost(), "File 3 : %d, %s, %d", list[2]->get()->getID(), list[2]->get()->getName().c_str(), list[2]->get()->getSize());
    LOGS_I(component->getHost(), "File 4 : %d, %s, %d", list[3]->get()->getID(), list[3]->get()->getName().c_str(), list[3]->get()->getSize());

    return true;
}

void TestApp::testSendProcessFiles(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_FILEBINARY, "TEST_FILEBINARY");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_FILEBINARY, processProcessFilesMsg);

    auto target = std::make_shared<ComponentUnit>(COMP_NODE, node->getHost()->getArch(), node->getHost()->getID(),
                         node->getHost()->getAddress(COMP_COLLECTOR));
    sendProcessFiles((TypeComponent&) collector, target);
}
