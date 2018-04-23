//
// Created by akgunduz on 19.03.2018.
//

#include "Jobs.h"

Jobs::Jobs() {

}

Jobs::~Jobs() {

    clear();
}

bool Jobs::add(Job *job) {

    jobs[job->getJobDir()] = job;
    return true;
}

bool Jobs::add(COMPONENT host, const char *path) {

    jobs[path] = new Job(host, path);
    return true;
}

//bool Jobs::addList(Jobs *jobs, bool init) {
//
//    if (init) {
//        clear();
//    }
//
//    this->jobs.insert(this->jobs.end(),
//                      jobs->getJobs()->begin(), jobs->getJobs()->end());
//    return true;
//}

bool Jobs::addList(COMPONENT host, const char *path, bool init) {

    if (init) {
        clear();
    }

    std::vector<std::string> dirList = Util::getDirList(path, JOB_DIR_PREFIX);
    for (int i = 0; i < dirList.size(); i++) {
        add(host, dirList[i].c_str());
    }
    return true;
}

Job* Jobs::get(const char* path) {

    TypeJobList::iterator search = jobs.find(path);
    if (search == jobs.end()) {
        return NULL;
    }

    return jobs[path];
}

Job* Jobs::get(int index) {

    for (TypeJobList::iterator i = jobs.begin(); i != jobs.end(); i++) {
        if (index-- == 0) {
            return i->second;
        }
    }

    return NULL;
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
//    for (; i < getExecutorCount(); i++) {
//
//        NodeInfo node = nodes.get(jobs[i]);
//        if (node.getAddress() == 0) {
//            break;
//        }
//    }
//
//    if (i == getExecutorCount()) {
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

    for (TypeJobList::iterator i = jobs.begin(); i != jobs.end(); i++) {

        delete i->second;
    }

    jobs.clear();
    return true;
}

bool Jobs::isEmpty() {

    return jobs.empty();
}
//
//std::vector<Job*> *Jobs::getJobs() {
//
//    return &jobs;
//}

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
