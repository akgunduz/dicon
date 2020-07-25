//
// Created by akgunduz on 26.10.2015.
//

#include <external/miniz/miniz.h>
#include "JobItem.h"
#include "ParameterItem.h"
#include "Util.h"

long JobItem::jobID = 1;

JobItem::JobItem(const ComponentObject& host, const char* jobZipFile, long _jobID)
        : FileItem(host, _jobID, _jobID, JOB_FILE) {

    contentTypes[CONTENT_NAME] = new JsonType(CONTENT_NAME, "name", this, parseNameNode);
    contentTypes[CONTENT_FILE] = new JsonType(CONTENT_FILE, "files", this, parseFileNode);
    contentTypes[CONTENT_PARAM] = new JsonType(CONTENT_PARAM, "parameters", this, parseParamNode);
    contentTypes[CONTENT_PROCESS] = new JsonType(CONTENT_PROCESS, "processes", this, parseProcessNode);

    if (!extract(jobZipFile, _jobID)) {
        LOGS_E(getHost(), "Job can not extracted from Zip file!!!");
        return;
    }

    if (!parse()) {
        LOGS_E(getHost(), "Job file could not parsed!!!");
        return;
    }

    for (int i = 0; i < getProcessCount(); i++) {
        getProcess(i)->parse(this);
    }

    if (!createDependencyMap()){
        LOGS_E(getHost(), "Dependency Loop Detected in the Job!!!");
        return;
    }

    for (int i = 0; i < getProcessCount(); i++) {
        if (getProcess(i)->getState() == PROCESS_STATE_DEPENDENT && getProcess(i)->check()) {
            getProcess(i)->setState(PROCESS_STATE_READY);
        }
    }
}

JobItem::~JobItem() {

    for (auto & contentType : contentTypes) {
        delete contentType.second;
    }

    for (auto & content : contentList) {
        for (auto item : content) {
            delete item;
        }
    }
}

ContentItem* JobItem::getContent(int type, int index) const {

    return contentList[type][index];
}

int JobItem::getContentCount(int type) const {

    return (int) contentList[type].size();
}

void JobItem::reset() {

    for (int i = 0; i < contentList->size(); i++) {
        contentList[i].clear();
    }
}

bool JobItem::parse() {

    struct json_object* node = json_object_from_file(
            Util::getAbsRefPath(getHost().getRootPath(), getID(), getName()).c_str());
    if (node == nullptr){
        LOGS_E(getHost(), "Invalid JSON File");
        return false;
    }

    auto* header = (struct json_object*)json_object_get_object(node)->head->v;

    json_object_object_foreach(header, key, val) {

        for (auto & contentType : contentTypes) {

            JsonType* type = contentType.second;
            if (strcmp(type->name, key) == 0) {
                (type->parser)(type->parent, val);
                break;
            }
        }
    }

    return true;
}

bool JobItem::parseNameNode(JobItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_string) {
        LOGS_E(parent->getHost(), "Invalid JSON Name Node");
        return false;
    }

    const char *name = json_object_get_string(node);

    parent->setJobName(name);

    return true;
}

bool JobItem::parseFileNode(JobItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOGS_E(parent->getHost(), "Invalid JSON Files Node");
        return false;
    }

    for (int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOGS_E(parent->getHost(), "Invalid JSON Files Node");
            return false;
        }

        const char* path = json_object_get_string(child);

        auto *content = new FileItem(parent->getHost(), i, parent->getID(), path);

        parent->contentList[CONTENT_FILE].emplace_back(content);

    }
    return true;
}

bool JobItem::parseParamNode(JobItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOGS_E(parent->getHost(), "Invalid JSON Parameter Node");
        return false;
    }

    for (unsigned int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOGS_E(parent->getHost(), "Invalid JSON Parameter Node");
            return false;
        }

        const char *param = json_object_get_string(child);

        auto *content = new ParameterItem(parent->getHost(), param);

        parent->contentList[CONTENT_PARAM].emplace_back(content);
    }

    return true;
}

bool JobItem::parseProcessNode(JobItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOGS_E(parent->getHost(), "Invalid JSON Process Node");
        return false;
    }

    for (unsigned int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOGS_E(parent->getHost(), "Invalid JSON Process Node");
            return false;
        }

        const char *process = json_object_get_string(child);

        auto *content = new ProcessItem(parent->getHost(), i + 1, parent->getID(), process);

        parent->contentList[CONTENT_PROCESS].emplace_back(content);
    }

    return true;
}

const char *JobItem::getJobName() const {

    return jobName;
}

void JobItem::setJobName(const char *_jobName) {

    strncpy(jobName, _jobName, NAME_MAX);
}

int JobItem::getProcessCount() const {

    return getContentCount(CONTENT_PROCESS);
}

int JobItem::getProcessCount(PROCESS_STATE state) const {

    int count = 0;

    for (int i = 0; i < getProcessCount(); i++) {

        if (getProcess(i)->getState() == state) {
            count++;
        }
    }

    return count;
}

ProcessItem* JobItem::getProcess(int index) const {

    return (ProcessItem*)getContent(CONTENT_PROCESS, index);
}

ProcessItem* JobItem::getProcessByID(long id) const {

    for (int i = 0; i < getProcessCount(); i++) {

        auto *process = getProcess(i);

        if (process->getID() == id) {
            return process;
        }
    }

    return nullptr;
}

int JobItem::getFileCount() const {

    return getContentCount(CONTENT_FILE);
}

FileItem* JobItem::getFile(int index) const {

    return (FileItem*)getContent(CONTENT_FILE, index);
}

FileItem* JobItem::getFileByID(long id) const {

    for (int i = 0; i < getFileCount(); i++) {

        auto *file = getFile(i);

        if (file->getID() == id) {
            return file;
        }
    }

    return nullptr;
}

int JobItem::getParameterCount() const {

    return getContentCount(CONTENT_PARAM);
}

ParameterItem* JobItem::getParameter(int index) const {

    return (ParameterItem*)getContent(CONTENT_PARAM, index);
}

ProcessItem* JobItem::assignNode(ComponentObject &node) {

    mutex.lock();

    for (int i = 0; i < getProcessCount(); i++) {

        ProcessItem* process = getProcess(i);
        if (process->getState() == PROCESS_STATE_READY) {
            process->setAssigned(node.getID());
            process->setState(PROCESS_STATE_STARTED);
            mutex.unlock();
            return process;
        }
    }

    mutex.unlock();

    return nullptr;
}

int JobItem::getByOutput(int index) const {

    for (int i = 0; i < getProcessCount(); i++) {

        auto *process = getProcess(i);

        for (auto processFile : process->getFileList()) {

            if (processFile.isOutput() && processFile.get()->getID() == index) {
                return i;
            }
        }
    }

    return -1;
}

bool JobItem::createDependencyMap() {

    int depth[getFileCount()];
    std::vector<int> adj[getFileCount()];
    std::list<int> initial, final;

    bzero(depth, (size_t)getFileCount() * sizeof(int));

    for (int i = 0; i < getProcessCount(); i++) {

        TypeFileList outList, depList;

        for (auto processFile : getProcess(i)->getFileList()) {

            processFile.isOutput() ?
                outList.emplace_back(processFile.get())
                : depList.emplace_back(processFile.get());
        }

        long id = outList[0]->getID();

        for (auto & j : depList) {

            //TODO will be updated with multi output files

            adj[j->getID()].emplace_back(id);

            depth[id]++;
        }
    }

    for (int i = 0; i < getFileCount(); i++) {

        if (depth[i] == 0) {
            initial.emplace_back(i);
        }
    }

    while(!initial.empty()) {

        int current = initial.front();
        initial.pop_front();

        final.emplace_back(current);

        for (int & i : adj[current]) {

            depth[i] -= 1;
            if (depth[i] == 0) {
                initial.emplace_back(i);
            }
        }
    }

    for (int i = 0; i < getFileCount(); i++) {

        if (depth[i] > 0) {
            return false;
        }
    }

    int orderedProcessIndex = 0;

    for (auto outputIndex : final) {

        int processIndex = getByOutput(outputIndex);
        if (processIndex == -1) {
            continue;
        }

        getProcess(processIndex)->setID(orderedProcessIndex + 1);

        std::swap(contentList[CONTENT_PROCESS][orderedProcessIndex],
                contentList[CONTENT_PROCESS][processIndex]);

        orderedProcessIndex++;
    }

    return true;
}

int JobItem::updateDependency(long id, int &totalCount) {

    int readyCount = 0;
    totalCount = 0;

    mutex.lock();

    for (int i = 0; i < getProcessCount(); i++) {

        auto *process = getProcess(i);

        if (process->getID() == id) {
            process->setState(PROCESS_STATE_ENDED);
        }

        if (process->getState() != PROCESS_STATE_ENDED) {
            totalCount++;
        }

        if (process->getState() == PROCESS_STATE_DEPENDENT && process->check()) {
            process->setState(PROCESS_STATE_READY);
            readyCount++;
        }
    }

    mutex.unlock();

    return readyCount;
}

bool JobItem::extract(const char *zipFile, long& _jobID) {

    mz_zip_archive zip_archive;
    mz_zip_archive_file_stat file_stat;
    char absPath[PATH_MAX];

    memset(&zip_archive, 0, sizeof(zip_archive));

    mz_bool status = mz_zip_reader_init_file(&zip_archive, zipFile, 0);
    if (!status) {
        LOGS_E(getHost(), "mz_zip_reader_init_file() failed!");
        return false;
    }

    int fileCount = mz_zip_reader_get_num_files(&zip_archive);
    if (fileCount < 1) {
        LOGS_E(getHost(), "mz_zip_reader_get_num_files() failed!");
        mz_zip_reader_end(&zip_archive);
        return false;
    }

    sprintf(absPath, "%s/%ld", getHost().getRootPath(), _jobID);

    Util::removePath(absPath);

    for (int i = 0; i < fileCount; i++) {

        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
            continue;
        }

        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
            printf("mz_zip_reader_file_stat() failed!\n");
            mz_zip_reader_end(&zip_archive);
            return false;
        }

        sprintf(absPath, "%s/%ld/%s", getHost().getRootPath(), _jobID, file_stat.m_filename);

        Util::mkPath(absPath);

        status = mz_zip_reader_extract_to_file(&zip_archive, i, absPath, 0);
        if (!status) {
            printf("mz_zip_reader_extract_file_to_file() failed!\n");
            mz_zip_reader_end(&zip_archive);
            return false;
        }
    }

    mz_zip_reader_end(&zip_archive);

    return true;
}

bool JobItem::check() {

    if (!FileItem::check()) {
        return false;
    }

    for (int i = 0; i < getProcessCount(); i++) {

        auto *process = getProcess(i);

        if (!process->check()) {
            return false;
        }
    }

    return true;
}

long JobItem::getDuration() const {

    return duration;
}

void JobItem::setDuration(long _duration) {

    duration = _duration;
}
