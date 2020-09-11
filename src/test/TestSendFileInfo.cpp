//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendFileInfo(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(target->getType()), (MSG_TYPE)MSG_TYPE_TEST_FILE_INFO, STREAM_FILE_INFO);

    auto file = std::make_shared<FileItem>(owner->getHost(), 99, ".", TEST_JOB_FILE);

    file->check();

    msg->getData().setFile(file);

    owner->send(target, std::move(msg));
}

bool processFileInfoMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    auto file = msg->getData().getFile();

    LOGS_I(component->getHost(), "Message File Info has came from : %s ",
           ComponentType::getName(owner->getType()));
    LOGS_I(component->getHost(), "File[%d] => Name : %s", file->getID(),
           file->getName().c_str());

    return true;
}

void TestApp::testSendFileInfo(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_FILE_INFO, "TEST_FILE_INFO");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_FILE_INFO, processFileInfoMsg);

    auto target = std::make_shared<ComponentUnit>(COMP_NODE, node->getHost()->getArch(), node->getHost()->getID(),
                                                  node->getHost(COMP_COLLECTOR)->getAddress());
    sendFileInfo((TypeComponent&) collector, target);
}
