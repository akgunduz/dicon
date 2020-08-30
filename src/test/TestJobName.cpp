//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

void sendJobName(const TypeComponent& owner, const TypeComponentUnit& target) {

    auto msg = std::make_unique<Message>(owner->getHost(), target, (MSG_TYPE)MSG_TYPE_TEST_JOBNAME);

    auto job = std::make_shared<JobItem>(owner->getHost(), TEST_JOB_PATH, JobItem::jobID++);

    msg->getData().setStreamType(STREAM_JOB);
    msg->getData().setJobName(job->getJobName());

    owner->send(target, std::move(msg));
}

bool processJobNameMsg(const TypeComponent& component, const TypeComponentUnit& owner, TypeMessage msg) {

    const std::string& jobName = msg->getData().getJobName();

    LOGS_I(component->getHost(), "Message JobName has came from : %s => JobName : %s",
           ComponentType::getName(owner->getType()), jobName.c_str());

    return true;
}

void TestApp::testJobName(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {

    MessageType::addMsg(MSG_TYPE_TEST_JOBNAME, "TEST_JOBNAME");

    node->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_JOBNAME, processJobNameMsg);

    auto target = std::make_shared<ComponentUnit>(COMP_NODE, node->getHost()->getArch(), node->getHost()->getID(),
                         node->getHost()->getAddress(COMP_COLLECTOR));

    sendJobName((TypeComponent&) collector, target);
}
