//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendJobName(Component *owner, ComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_JOBNAME);

    auto job = std::make_unique<JobItem>(owner->getHost(), "../sample/Job1_x86_linux.zip", JobItem::jobID++);

    msg->getData().setStreamFlag(STREAM_JOB);
    msg->getData().setJobName(job->getJobName());

    owner->send(target, std::move(msg));
}

bool processJobNameMsg(Component* component, ComponentUnit& owner, TypeMessage msg) {

    const char* jobName = msg->getData().getJobName();

    LOGS_I(component->getHost(), "Message JobName has came from : %s => JobName : %s", ComponentType::getName(owner.getType()), jobName);

    return true;
}

void TestApp::testJobName(Distributor* distributor, Collector* collector, Node* node) {

    MessageTypes::addMsg(MSG_TYPE_TEST_JOBNAME, "TEST_JOBNAME");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_JOBNAME, processJobNameMsg);

    ComponentUnit target(COMP_NODE, node->getHost().getArch(), node->getHost().getID(), node->getHost().getAddress(COMP_COLLECTOR));

    sendJobName(collector, target);
}
