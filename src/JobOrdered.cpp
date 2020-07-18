//
// Created by akgun on 5.07.2020.
//

#include "Job.h"

const ProcessInfo& Job::getProcess(int index) const {

    return processList[index];
}

int Job::getProcessCount() const {

    return processList.size();
}

int Job::updateDependency(int id, int &totalCount) {

    int readyCount = 0;
    totalCount = 0;

    mutex.lock();

    for (auto &process : processList) {
        if (process.getID() == id) {
            process.setState(PROCESS_STATE_ENDED);
        }

        if (process.getState() != PROCESS_STATE_ENDED) {
            totalCount++;
        }

        if (process.getState() == PROCESS_STATE_DEPENDENT
                && process.get().isValid()) {
            process.setState(PROCESS_STATE_READY);
            readyCount++;
        }
    }

    mutex.unlock();

    return readyCount;
}

const ProcessInfo& Job::assignNode(ComponentObject &node) {

    mutex.lock();

    for (auto &process : processList) {
        if (process.getState() == PROCESS_STATE_READY) {
            process.setAssigned(node.getID());
            process.setState(PROCESS_STATE_STARTED);
            mutex.unlock();
            return process;
        }
    }

    mutex.unlock();

    return ProcessInfo::invalid;
}

ProcessItem* Job::getByOutput(int index) {

    for (int i = 0; i < jobFile->getProcessCount(); i++) {

        auto *process = jobFile->getProcess(i);

        TypeFileInfoList list = FileInfo::getFileList(process->getFileList(), true);
        if (list.empty()) {
            continue;
        }

        if (list[0].get()->getID() == index) {
            return process;
        }
    }

    return nullptr;
}

bool Job::createDependencyMap() {

    int depth[jobFile->getFileCount()];
    std::vector<int> adj[jobFile->getFileCount()];
    std::list<int> initial, final;

    bzero(depth, (size_t)jobFile->getFileCount() * sizeof(int));

    for (int i = 0; i < jobFile->getProcessCount(); i++) {

        TypeFileList outList, depList;

        const TypeFileInfoList &fileList = jobFile->getProcess(i)->getFileList();

        for (auto & j : fileList) {

            FileItem* file = j.get();

            j.isOutput() ? outList.push_back(file) : depList.push_back(file);
        }

        int id = outList[0]->getID();

        for (auto & j : depList) {

            //TODO will be updated with multi output files

            adj[j->getID()].push_back(id);

            depth[id]++;
        }
    }

    for (int i = 0; i < jobFile->getFileCount(); i++) {

        if (depth[i] == 0) {
            initial.push_back(i);
        }
    }

    while(!initial.empty()) {

        int current = initial.front();
        initial.pop_front();

        final.push_back(current);

        for (int & i : adj[current]) {

            depth[i] -= 1;
            if (depth[i] == 0) {
                initial.push_back(i);
            }
        }
    }

    for (int i = 0; i < jobFile->getFileCount(); i++) {

        if (depth[i] > 0) {
            return false;
        }
    }

    processList.clear();

    for (auto process : final) {

        auto *content = getByOutput(process);
        if (content) {
            processList.emplace_back(ProcessInfo((int)processList.size() + 1, *content));
        }
    }

    return true;
}

ComponentObject &Job::getHost() {

    return host;
}

int Job::getFileCount() {

    return jobFile->getFileCount();
}

int Job::getProcessCount() {

    return jobFile->getProcessCount();
}

const char *Job::getName() {

    return jobFile->getName();
}

FileItem *Job::getFile(int index) {

    return jobFile->getFile(index);
}
