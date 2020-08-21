//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendFileBinary(TypeComponent& owner, ComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_FILEBINARY);

    auto job = std::make_unique<JobItem>(owner->getHost(), "../sample/Job1_x86_linux.zip", JobItem::jobID++);

    TypeProcessFileList list = job->getProcess(0)->getFileList();
    for (auto processFile : list) {
        processFile.get()->check();
    }

    msg->getData().setStreamFlag(STREAM_FILEBINARY);
    msg->getData().addFileList(job->getProcess(0)->getID(), list);

    owner->send(target, std::move(msg));
}

bool processFileBinaryMsg(TypeComponent& component, ComponentUnit& owner, TypeMessage msg) {

    TypeProcessFileList list = msg->getData().getFileList();

    LOGS_I(component->getHost(), "Message File Binary has came from : %s with assigned Process : %d",
           ComponentType::getName(owner.getType()), msg->getData().getFileProcess());
    LOGS_I(component->getHost(), "File 1 : %d, %s, %d", list[0].get()->getID(), list[0].get()->getName(), list[0].get()->getSize());
    LOGS_I(component->getHost(), "File 2 : %d, %s, %d", list[1].get()->getID(), list[1].get()->getName(), list[1].get()->getSize());
    LOGS_I(component->getHost(), "File 3 : %d, %s, %d", list[2].get()->getID(), list[2].get()->getName(), list[2].get()->getSize());
    LOGS_I(component->getHost(), "File 4 : %d, %s, %d", list[3].get()->getID(), list[3].get()->getName(), list[3].get()->getSize());

    return true;
}

void TestApp::testFileBinary(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_FILEBINARY, "TEST_FILEBINARY");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_FILEBINARY, processFileBinaryMsg);

    ComponentUnit target(COMP_NODE, node->getHost().getArch(), node->getHost().getID(),
                         node->getHost().getAddress(COMP_COLLECTOR));
    sendFileBinary((TypeComponent&) collector, target);
}
