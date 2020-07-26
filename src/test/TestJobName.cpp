//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendJobName(Component *owner, const ComponentObject& target) {

    auto *msg = new Message(owner->getHost(), target.getType(), (MSG_TYPE)MSG_TYPE_TEST_JOBNAME);

    auto *job = new JobItem(owner->getHost(), "../sample/Job1_macos.zip", JobItem::jobID++);

    msg->getData().setStreamFlag(STREAM_JOB);
    msg->getData().setJobName(job->getJobName());

    owner->send(target, msg);
}

bool processJobNameMsg(Component* component, const ComponentObject& owner, Message *msg) {

    const char* jobName = msg->getData().getJobName();

    LOGS_I(component->getHost(), "Message JobName has came from : %s => JobName : %s", owner.getName(), jobName);

    return true;
}

void TestApp::testJobName(Distributor* distributor, Collector* collector, Node* node) {

    MessageTypes::addMsg(MSG_TYPE_TEST_JOBNAME, "TEST_JOBNAME");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_JOBNAME, processJobNameMsg);

    ComponentObject target(node->getHost(), COMP_COLLECTOR);
    sendJobName(collector, target);
}
