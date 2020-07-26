//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendProcess(Component *owner, const ComponentObject& target) {

    auto *msg = new Message(owner->getHost(), target.getType(), (MSG_TYPE)MSG_TYPE_TEST_PROCESS);

    auto *job = new JobItem(owner->getHost(), "../sample/Job1_macos.zip", JobItem::jobID++);

    job->getProcess(0)->check();

    msg->getData().setStreamFlag(STREAM_PROCESS);
    msg->getData().addProcess(job->getProcess(0));

    owner->send(target, msg);
}

bool processProcessMsg(Component* component, const ComponentObject& owner, Message *msg) {

    ProcessItem *processItem = msg->getData().getProcess(0);

    LOGS_I(component->getHost(), "Message Process has came from : %s", owner.getName());

    return true;
}

void TestApp::testProcess(Distributor* distributor, Collector* collector, Node* node) {

    MessageTypes::addMsg(MSG_TYPE_TEST_PROCESS, "TEST_PROCESS");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_PROCESS, processProcessMsg);

    ComponentObject target(node->getHost(), COMP_COLLECTOR);
    sendProcess(collector, target);
}
