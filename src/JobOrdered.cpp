//
// Created by akgun on 5.07.2020.
//

#include "Job.h"

//ProcessItem *const Job::getOrderedExecution(int index) const {
//
//    return orderedList[index].get();
//}
//
//const PROCESS_STATE Job::getOrderedState(int index) const {
//
//    return orderedList[index].getState();
//}

//void Job::setOrderedState(int index, PROCESS_STATE state) {
//
//    orderedList[index].setState(state);
//}

//ProcessInfo& Job::getUnServed() {
//
//    for (int i = 0; i < getOrderedCount(); i++) {
//
//        if (getOrderedState(i) == PROCESS_STATE_STARTED ||
//            getOrderedState(i) == PROCESS_STATE_ENDED) {
//            continue;
//        }
//
//        if (!getOrderedExecution(i)->isValid()) {
//            continue;
//        }
//
//        setOrderedState(i, PROCESS_STATE_STARTED);
//
//        return getOrdered(i);
//    }
//
//    return ProcessInfo::invalid;
//}

//long Job::getUnServedCount() {
//
//    int count = 0;
//
//    for (int i = 0; i < getOrderedCount(); i++) {
//
//        if (getOrderedState(i) == PROCESS_STATE_STARTED ||
//            getOrderedState(i) == PROCESS_STATE_ENDED) {
//            continue;
//        }
//
//        if (!getOrderedExecution(i)->isValid()) {
//            continue;
//        }
//
//        count++;
//    }
//
//    return count;
//}

ProcessInfo& Job::getProcess(int index) {

    return processList[index];
}

int Job::getProcessCount() const {

    return processList.size();
}

int Job::getProcessCount(PROCESS_STATE state) {

    long count = 0;

    mutex.lock();

    for (auto process : processList) {
        if (process.getState() == state) {
            count++;
        }
    }

    mutex.unlock();

    return count;
}

bool Job::updateDependency() {

    bool modified = false;

    mutex.lock();

    for (auto process : processList) {
        if (process.getState() == PROCESS_STATE_DEPENDENT
                && process.get()->isValid()) {
            process.setState(PROCESS_STATE_READY);
            modified = true;
        }
    }

    mutex.unlock();

    return modified;
}

ProcessItem *Job::getByOutput(int index) {

    for (int i = 0; i < getExecutorCount(); i++) {

        auto *executor = getExecutor(i);

        TypeFileInfoList list = FileInfo::getFileList(executor->getFileList(), true);
        if (list.empty()) {
            continue;
        }

        if (list[0].get()->getID() == index) {
            return executor;
        }
    }

    return NULL;
}

bool Job::createDependencyMap() {

    int depth[getFileCount()];
    std::vector<int> adj[getFileCount()];
    std::list<int> initial, final;

    bzero(depth, (size_t)getFileCount() * sizeof(int));

    for (int i = 0; i < getExecutorCount(); i++) {

        TypeFileList outList, depList;

        TypeFileInfoList *fileList = getExecutor(i)->getFileList();

        for (int j = 0; j < fileList->size(); j++) {

            FileItem* file = fileList->at(j).get();

            fileList->at(j).isOutput() ? outList.push_back(file) : depList.push_back(file);
        }

        int id = outList[0]->getID();

        for (int j = 0; j < depList.size(); j++) {

            //TODO will be updated with multi output files

            adj[depList[j]->getID()].push_back(id);

            depth[id]++;
        }
    }

    for (int i = 0; i < getFileCount(); i++) {

        if (depth[i] == 0) {
            initial.push_back(i);
        }
    }

    while(!initial.empty()) {

        int current = initial.front();
        initial.pop_front();

        final.push_back(current);

        for (int i = 0; i < adj[current].size(); i++) {

            depth[adj[current][i]] -= 1;
            if (depth[adj[current][i]] == 0) {
                initial.push_back(adj[current][i]);
            }
        }
    }

    for (int i = 0; i < getFileCount(); i++) {

        if (depth[i] > 0) {
            return false;
        }
    }

    processList.clear();

    for (int i : final) {

        auto *content = getByOutput(i);
        if (content) {
            processList.emplace_back(ProcessInfo(processList.size() + 1, content));
        }
    }

    return true;
}
