//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendFileBinary(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_FILEBINARY);

    auto job = std::make_shared<JobItem>(owner->getHost(), TEST_JOB_PATH, JobItem::jobID++);

    auto file = job->getProcess(0)->getFileList()[0];

    file->get()->check();

    msg->getData().setStreamType(STREAM_FILESOLO);
    msg->getData().setProcess(job->getProcess(0)->getID(), file);

    owner->send(target, std::move(msg));
}

bool processFileBinaryMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    auto file = msg->getData().getProcess()->getFileList()[0];

    LOGS_I(component->getHost(), "Message File Binary has came from : %s with assigned Process : %d",
           ComponentType::getName(owner->getType()), msg->getData().getProcess()->getID());
    LOGS_I(component->getHost(), "File : %d, %s, %d", file->get()->getID(),
           file->get()->getName().c_str(), file->get()->getSize());

    return true;
}

void TestApp::testFileBinary(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_FILEBINARY, "TEST_FILEBINARY");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_FILEBINARY, processFileBinaryMsg);

    auto target = std::make_shared<ComponentUnit>(COMP_NODE, node->getHost()->getArch(), node->getHost()->getID(),
                         node->getHost()->getAddress(COMP_COLLECTOR));
    sendFileBinary((TypeComponent&) collector, target);
}
