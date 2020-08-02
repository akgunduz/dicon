//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendFileInfo(Component *owner, ComponentUnit& target) {

    auto *msg = new Message(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_FILEINFO);

    auto *job = new JobItem(owner->getHost(), "../sample/Job1_macos.zip", JobItem::jobID++);

    TypeProcessFileList list = job->getProcess(0)->getFileList();

    msg->getData().setStreamFlag(STREAM_FILEINFO);
    msg->getData().addFileList(job->getProcess(0)->getID(), list);

    owner->send(target, msg);
}

bool processFileInfoMsg(Component* component, ComponentUnit& owner, Message *msg) {

    TypeProcessFileList list = msg->getData().getFileList();

    LOGS_I(component->getHost(), "Message File Info has came from : %s", ComponentType::getName(owner.getType()));
    LOGS_I(component->getHost(), "File 1 : %d, %s", list[0].get()->getID(), list[0].get()->getName());
    LOGS_I(component->getHost(), "File 2 : %d, %s", list[1].get()->getID(), list[1].get()->getName());
    LOGS_I(component->getHost(), "File 3 : %d, %s", list[2].get()->getID(), list[2].get()->getName());
    LOGS_I(component->getHost(), "File 4 : %d, %s", list[3].get()->getID(), list[3].get()->getName());

    return true;
}

void TestApp::testFileInfo(Distributor* distributor, Collector* collector, Node* node) {

    MessageTypes::addMsg(MSG_TYPE_TEST_FILEINFO, "TEST_FILEINFO");

    collector->addStaticProcessHandler(COMP_NODE, (MSG_TYPE)MSG_TYPE_TEST_FILEINFO, processFileInfoMsg);

    ComponentUnit target(COMP_COLLECTOR, collector->getHost().getID(), collector->getHost().getAddress(COMP_NODE));
    sendFileInfo(node, target);
}