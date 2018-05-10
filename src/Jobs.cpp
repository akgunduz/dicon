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

    jobs[job->getJobID()] = job;
    return true;
}

bool Jobs::add(ComponentObject host, const char *path) {

    auto *job = new Job(host, path);
    jobs[job->getJobID()] = job;

    return true;
}

bool Jobs::addPath(ComponentObject host, bool init) {

    if (init) {
        clear();
    }

    std::vector<std::string> dirList = Util::getDirList(host.getRootPath(), JOB_DIR_PREFIX);

    for (int i = 0; i < dirList.size(); i++) {
        add(host, dirList[i].c_str());
    }

    return true;
}

Job* Jobs::get(TypeUUID &id) {

    auto search = jobs.find(id);
    if (search == jobs.end()) {
        return NULL;
    }

    return jobs[id];
}

Job* Jobs::get(int index) {

    for (auto i = jobs.begin(); i != jobs.end(); i++) {
        if (index-- == 0) {
            return i->second;
        }
    }

    return NULL;
}

bool Jobs::clear() {

    for (auto i = jobs.begin(); i != jobs.end(); i++) {

        delete i->second;
    }

    jobs.clear();
    return true;
}

bool Jobs::isEmpty() {

    return jobs.empty();
}

unsigned long Jobs::getCount() {

    return jobs.size();
}
