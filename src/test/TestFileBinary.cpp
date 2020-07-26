//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendFileBinary(Component *owner, const ComponentObject& target) {

    auto *msg = new Message(owner->getHost(), target.getType(), (MSG_TYPE)MSG_TYPE_TEST_FILEBINARY);

    auto *job = new JobItem(owner->getHost(), "../sample/Job1_macos.zip", JobItem::jobID++);

    TypeProcessFileList list = job->getProcess(0)->getFileList();
    for (auto processFile : list) {
        processFile.get()->check();
    }

    msg->getData().setStreamFlag(STREAM_FILEBINARY);
    msg->getData().addFileList(job->getProcess(0)->getID(), list);

    owner->send(target, msg);
}

bool processFileBinaryMsg(Component* component, const ComponentObject& owner, Message *msg) {

    TypeProcessFileList list = msg->getData().getFileList();

    LOGS_I(component->getHost(), "Message File Binary has came from : %s with assigned Process : %d",
            owner.getName(), msg->getData().getFileProcess());
    LOGS_I(component->getHost(), "File 1 : %d, %s, %d", list[0].get()->getID(), list[0].get()->getName(), list[0].get()->getSize());
    LOGS_I(component->getHost(), "File 2 : %d, %s, %d", list[1].get()->getID(), list[1].get()->getName(), list[1].get()->getSize());
    LOGS_I(component->getHost(), "File 3 : %d, %s, %d", list[2].get()->getID(), list[2].get()->getName(), list[2].get()->getSize());
    LOGS_I(component->getHost(), "File 4 : %d, %s, %d", list[3].get()->getID(), list[3].get()->getName(), list[3].get()->getSize());

    return true;
}

void TestApp::testFileBinary(Distributor* distributor, Collector* collector, Node* node) {

    MessageTypes::addMsg(MSG_TYPE_TEST_FILEBINARY, "TEST_FILEBINARY");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_FILEBINARY, processFileBinaryMsg);

    ComponentObject target(node->getHost(), COMP_COLLECTOR);
    sendFileBinary(collector, target);
}
