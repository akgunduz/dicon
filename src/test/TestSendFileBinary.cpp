//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendFileBinary(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_FILEBINARY, STREAM_FILEBINARY);

    auto file = std::make_shared<FileItem>(owner->getHost(), 99, ".", TEST_JOB_FILE);

    file->check();

    msg->getData().setFile(file);

    owner->send(target, std::move(msg));
}

bool processFileBinaryMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    auto file = msg->getData().getFile();

    LOGS_I(component->getHost(), "Message File Binary has came from : %s ",
           ComponentType::getName(owner->getType()));
    LOGS_I(component->getHost(), "File[%d] => Name : %s, Size : %d", file->getID(),
           file->getName().c_str(), file->getSize());

    return true;
}

void TestApp::testSendFileBinary(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_FILEBINARY, "TEST_FILEBINARY");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_FILEBINARY, processFileBinaryMsg);

    auto target = std::make_shared<ComponentUnit>(COMP_NODE, node->getHost()->getArch(), node->getHost()->getID(),
                         node->getHost()->getAddress(COMP_COLLECTOR));
    sendFileBinary((TypeComponent&) collector, target);
}
