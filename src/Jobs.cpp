//
// Created by akgunduz on 19.03.2018.
//

#include "Jobs.h"

Jobs::Jobs() {

}

Jobs::~Jobs() {

    clear();
}

bool Jobs::addJob(Job *job) {

    jobs.push_back(job);
    return true;
}

bool Jobs::addJob(Unit host, const char *path) {

    jobs.push_back(new Job(host, path));
    return true;
}


bool Jobs::addJobList(Jobs *jobs, bool init) {

    if (init) {
        clear();
    }

    this->jobs.insert(this->jobs.end(),
                      jobs->getJobs()->begin(), jobs->getJobs()->end());
    return true;
}

bool Jobs::addJobList(Unit host, const char *path, bool init) {

    if (init) {
        clear();
    }

    std::vector<std::string> dirList = Util::getDirList(path, JOB_DIR_PREFIX);
    for (int i = 0; i < dirList.size(); i++) {
        addJob(host, dirList[i].c_str());
    }
    return true;
}

Job* Jobs::getJob(int index) {

    if (index >= getCount()) {
        LOG_W("There is only %d jobs available", getCount());
        return NULL;
    }

    return jobs[index];
}

//Job* Jobs::getJobByNode(NodeInfo node) {
//
//    return nodes.get(node);
//}
//
//Job* Jobs::getJobByAddress(long address) {
//
//    return nodes.get(address);
//}
//
//NodeInfo Jobs::getNodeByAddress(long address) {
//
//    return nodes.getNode(address);
//}

//Job* Jobs::getJobUnServed() {
//
//    int i = 0;
//    for (; i < getCount(); i++) {
//
//        NodeInfo node = nodes.get(jobs[i]);
//        if (node.getAddress() == 0) {
//            break;
//        }
//    }
//
//    if (i == getCount()) {
//        LOG_W("No job found");
//        return NULL;
//    }
//
//    return jobs[i];
//}
//
//bool Jobs::reset() {
//
//    nodes.clear();
//    return true;
//}

bool Jobs::clear() {

    for (std::vector<Job*>::iterator i = jobs.begin(); i != jobs.end(); i++) {

        delete *i;
    }

 //   nodes.clear();
    jobs.clear();
    return true;
}

bool Jobs::isEmpty() {

    return jobs.size() == 0;
}

std::vector<Job*> *Jobs::getJobs() {

    return &jobs;
}

unsigned long Jobs::getCount() {

    return jobs.size();
}

//bool Jobs::attachNode(Job *job, NodeInfo node) {
//
//    nodes.add(job, node);
//
//    return true;
//
//}
//
//bool Jobs::detachNode(Job *job) {
//
//    nodes.remove(job);
//
//    return true;
//}
