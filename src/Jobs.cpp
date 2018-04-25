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

    jobs[std::string(job->getJobDir())] = job;
    return true;
}

bool Jobs::add(COMPONENT host, const char *path) {

    jobs[std::string(path)] = new Job(host, path);
    return true;
}

bool Jobs::addPath(COMPONENT host, const char *path, bool init) {

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

    auto search = jobs.find(path);
    if (search == jobs.end()) {
        return NULL;
    }

    return jobs[std::string(path)];
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
