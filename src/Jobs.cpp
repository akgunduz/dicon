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


Job* Jobs::getJobByID(long id) {

    int i = 0;
    for (; i < getCount(); i++) {
        if (jobs[i]->getID() != id) {
            continue;
        }
        break;
    }

    if (i == getCount()) {
        LOG_W("No job found with id %ld", id);
        return nullptr;
    }

    return jobs[i];
}

Job* Jobs::getJobByAddress(long address) {

    int i = 0;
    for (; i < getCount(); i++) {
        if (jobs[i]->getAttachedNode().getAddress() != address) {
            continue;
        }
        break;
    }

    if (i == getCount()) {
        LOG_W("No job found attached to %ld", address);
        return nullptr;
    }

    return jobs[i];
}

Job* Jobs::getUnServedJob() {

    int i = 0;
    for (; i < getCount(); i++) {
        if (jobs[i]->getAttachedNode().getAddress() > 0) {
            continue;
        }
        break;
    }

    if (i == getCount()) {
        LOG_W("No unServed job remains.");
        return nullptr;
    }

    return jobs[i];
}

bool Jobs::resetStates() {

    for (int i = 0; i < getCount(); i++) {
        jobs[i]->getAttachedNode().reset();
    }

    return true;
}

bool Jobs::clear() {

    for (std::vector<Job*>::iterator i = jobs.begin(); i != jobs.end(); i++) {
        delete *i;
    }

    jobs.clear();
    return true;
}

bool Jobs::isEmpty() {

    return jobs.size() == 0;
}

std::vector<Job *> *Jobs::getJobs() {

    return &jobs;
}

unsigned int Jobs::getCount() {

    return jobs.size();
}
