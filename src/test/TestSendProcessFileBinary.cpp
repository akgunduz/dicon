//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendProcessFileBinary(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target->getType(),
                                         (MSG_TYPE)MSG_TYPE_TEST_PROCESS_FILE_BINARY, STREAM_PROCESS_FILE_BINARY);

    auto file = std::make_shared<FileItem>(owner->getHost(), 99, "1", TEST_JOB_FILE);

    auto processFile = std::make_shared<ProcessFile>(file, 888, false);

    processFile->get()->checkContent();

    msg->getData().setProcessFile(processFile);

    owner->send(target, std::move(msg));
}

bool processProcessFileBinaryMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    auto processFile = msg->getData().getProcessFile();

    LOGS_I(component->getHost(), "Message Process File Binary has came from : %s ",
           ComponentType::getName(owner->getType()));
    LOGS_I(component->getHost(), "Process[%d] File[%d] => Name : %s, Size : %d, Path : %s",
           processFile->getAssignedProcess(),
           processFile->get()->getID(), processFile->get()->getName().c_str(),
           processFile->get()->getSize(), processFile->get()->getPath().c_str());

    return true;
}

void TestApp::testSendProcessFileBinary(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_PROCESS_FILE_BINARY, "TEST_PROCESS_FILE_BINARY");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_PROCESS_FILE_BINARY, processProcessFileBinaryMsg);

    auto target = std::make_shared<ComponentUnit>(COMP_NODE, node->getHost()->getArch(), node->getHost()->getID(),
                                                  node->getHost()->getAddress(COMP_COLLECTOR));
    sendProcessFileBinary((TypeComponent&) collector, target);
}
