//
// Created by akgunduz on 26.10.2015.
//

#include <external/miniz/miniz.h>
#include "JobItem.h"

long JobItem::jobID = 1;

JobItem::JobItem(const TypeHostUnit& host, const std::filesystem::path& jobPath, long _jobID)
        : FileItem(host, _jobID, _jobID, JOB_FILE) {

    contentTypes[CONTENT_NAME] = std::make_unique<JsonType>(CONTENT_NAME, "name", this, parseNameNode);
    contentTypes[CONTENT_FILE] = std::make_unique<JsonType>(CONTENT_FILE, "files", this, parseFileNode);
    contentTypes[CONTENT_PARAM] = std::make_unique<JsonType>(CONTENT_PARAM, "parameters", this, parseParamNode);
    contentTypes[CONTENT_PROCESS] = std::make_unique<JsonType>(CONTENT_PROCESS, "processes", this, parseProcessNode);

    JOB_PATH pathType = checkPath(jobPath);

    if (pathType == JOBPATH_INVALID) {
        status = JOBSTATUS_PATH_INVALID;
        return;
    }

    if (pathType == JOBPATH_ZIP) {
        if (!extract(jobPath, _jobID)) {
         status = JOBSTATUS_ZIP_INVALID;
         return;
        }

    } else {
        char absPath[PATH_MAX];
        sprintf(absPath, "%s/%ld", getHost()->getRootPath().c_str(), _jobID);
        std::filesystem::copy(jobPath, absPath,
                std::filesystem::copy_options::recursive |
                std::filesystem::copy_options::update_existing);
    }

    if (!parse()) {
        status = JOBSTATUS_ZIP_INVALID;
        return;
    }

    for (int i = 0; i < getProcessCount(); i++) {

        getProcess(i)->parse(this);
    }

    status = createDependencyMap(errorFileIDList);

    setProcessStateByFile(errorFileIDList);
}

JobItem::~JobItem() = default;

TypeContentItem JobItem::getContent(int type, int index) const {

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

    std::filesystem::path jobFilePath = getHost()->getRootPath() / std::to_string(getID()) / getName();
    struct json_object* node = json_object_from_file(jobFilePath.c_str());

    if (node == nullptr){
        LOGS_E(getHost(), "Invalid JSON File");
        return false;
    }

    auto* header = (struct json_object*)json_object_get_object(node)->head->v;

    json_object_object_foreach(header, key, val) {

        for (auto & contentType : contentTypes) {

            if (contentType.second->name == key) {
                (contentType.second->parser)(contentType.second->parent, val);
                break;
            }
        }
    }

    json_object_put(node);

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

        auto content = std::make_shared<FileItem>(parent->getHost(), i + 1, parent->getID(), path);

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

        auto content = std::make_shared<ParameterItem>(parent->getHost(), i + 1, parent->getID(), param);

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

        auto content = std::make_shared<ProcessItem>(parent->getHost(), i + 1, parent->getID(), process);

        parent->contentList[CONTENT_PROCESS].emplace_back(content);
    }

    return true;
}

const std::string& JobItem::getJobName() const {

    return jobName;
}

void JobItem::setJobName(const std::string& _jobName) {

    jobName = _jobName;
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

TypeProcessItem JobItem::getProcess(int index) const {

    return std::static_pointer_cast<ProcessItem>(getContent(CONTENT_PROCESS, index));
}

TypeProcessItem JobItem::getProcessByID(long id) const {

    for (int i = 0; i < getProcessCount(); i++) {

        auto process = getProcess(i);

        if (process->getID() == id) {
            return process;
        }
    }

    return nullptr;
}

int JobItem::getFileCount() const {

    return getContentCount(CONTENT_FILE);
}

TypeFileItem JobItem::getFile(int index) const {

    return std::static_pointer_cast<FileItem>(getContent(CONTENT_FILE, index));
}

TypeFileItem JobItem::getFileByID(long id) const {

    for (int i = 0; i < getFileCount(); i++) {

        auto file = getFile(i);

        if (file->getID() == id) {
            return file;
        }
    }

    return nullptr;
}

int JobItem::getParameterCount() const {

    return getContentCount(CONTENT_PARAM);
}

TypeParameterItem JobItem::getParameter(int index) const {

    return std::static_pointer_cast<ParameterItem>(getContent(CONTENT_PARAM, index));
}

TypeProcessItem JobItem::assignNode(long nodeID) {

    mutex.lock();

    for (int i = 0; i < getProcessCount(); i++) {

        auto process = getProcess(i);
        if (process->getState() == PROCESS_STATE_READY) {
            process->setAssigned(nodeID);
            process->setState(PROCESS_STATE_STARTED);
            mutex.unlock();
            return process;
        }
    }

    mutex.unlock();

    return nullptr;
}

TypeProcessItem JobItem::reAssignNode(long oldNodeID, long newNodeID) {

    mutex.lock();

    for (int i = 0; i < getProcessCount(); i++) {

        auto process = getProcess(i);
        if (process->getAssigned() == oldNodeID &&
                process->getState() != PROCESS_STATE_ENDED) {
              process->setAssigned(newNodeID);
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

        auto process = getProcess(i);

        for (const auto& processFile : process->getFileList()) {

            if (processFile->isOutput() && processFile->get()->getID() == index) {
                return i;
            }
        }
    }

    return -1;
}

bool JobItem::setProcessIDByOutput(long outputID, long processID) {

    for (int i = 0; i < getProcessCount(); i++) {

        auto process = getProcess(i);

        for (const auto& processFile : process->getFileList()) {

            if (processFile->isOutput()
                    && processFile->get()->getID() == outputID
                    && process->getID() == 0) {

                process->setID(processID);

                return true;
            }
        }
    }

    return false;
}

bool JobItem::setProcessStateByFile(std::vector<long> &errorList) {

    for (int i = 0; i < getProcessCount(); i++) {

        auto process = getProcess(i);

        bool ready = true;

        bool errFound = false;

        for (const auto& processFile : process->getFileList()) {

            for (auto fileID : errorList) {

                if (processFile->get()->getID() == fileID) {

                    processFile->get()->setRequired(true);

                    errFound = true;
                }
            }

            if (processFile->isOutput()) {
                continue;
            }

            if (!processFile->get()->check()) {

                ready = false;
            }
        }

        if (errFound) {

            process->setState(PROCESS_STATE_INVALID);

        } else if (ready) {

            process->setState(PROCESS_STATE_READY);

        }
    }

    return true;
}

JOB_STATUS JobItem::createDependencyMap(std::vector<long>& reqList) {

    std::map<long, int> depth;
    std::map<long, std::vector<long>> adj;
    std::list<long> initial, final;

    for (int i = 0; i < getProcessCount(); i++) {

        std::vector<long> outList, depList;

        for (const auto& processFile : getProcess(i)->getFileList()) {

            processFile->isOutput() ?
                outList.emplace_back(processFile->get()->getID())
                : depList.emplace_back(processFile->get()->getID());
        }

        for (auto &outID : outList) {

            for (auto &depID : depList) {

                adj[depID].emplace_back(outID);

                depth[outID]++;
            }
        }

        getProcess(i)->setID(0);
    }

    for (int i = 0; i < getFileCount(); i++) {

        auto file = getFile(i);

        if (depth[file->getID()] == 0) {
            initial.emplace_back(file->getID());
            if (!file->check()) {
                reqList.emplace_back(file->getID());
            }
        }
    }

    if (!reqList.empty()) {
        return JOBSTATUS_MISSING_FILES;
    }

    while(!initial.empty()) {

        int current = initial.front();
        initial.pop_front();

        final.emplace_back(current);

        for (long & outID : adj[current]) {

            depth[outID] -= 1;
            if (depth[outID] == 0) {
                initial.emplace_back(outID);
            }
        }
    }

    for (int i = 0; i < getFileCount(); i++) {

        auto file = getFile(i);

        if (depth[file->getID()] > 0) {
            reqList.emplace_back(file->getID());
        }
    }

    if (!reqList.empty()) {
        return JOBSTATUS_DEPENDENCY_LOOP;
    }

    long processID = 1;

    for (auto outputID : final) {

        bool res = setProcessIDByOutput(outputID, processID);

        if (res) {
            processID++;
        }
    }

    std::sort(contentList[CONTENT_PROCESS].begin(), contentList[CONTENT_PROCESS].end(), compareContentID);

    return JOBSTATUS_OK;
}

int JobItem::updateDependency(long id, int &totalCount) {

    int readyCount = 0;
    totalCount = 0;

    mutex.lock();

    for (int i = 0; i < getProcessCount(); i++) {

        auto process = getProcess(i);

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

JOB_PATH JobItem::checkPath(const std::filesystem::path& jobPath) {

    if (!std::filesystem::exists(jobPath)) {

        LOGS_E(getHost(), "Invalid Path");
        return JOBPATH_INVALID;
    }

    if (std::filesystem::is_directory(jobPath)) {

        return JOBPATH_DIR;
    }

    if (jobPath.has_extension() && jobPath.extension() == ".zip") {

        return JOBPATH_ZIP;
    }

    return JOBPATH_INVALID;
}


bool JobItem::extract(const std::string& zipFile, long& _jobID) {

    mz_zip_archive zip_archive;
    mz_zip_archive_file_stat file_stat;

    memset(&zip_archive, 0, sizeof(zip_archive));

    mz_bool zipStatus = mz_zip_reader_init_file(&zip_archive, zipFile.c_str(), 0);
    if (!zipStatus) {
        LOGS_E(getHost(), "Zip reader init is failed!");
        return false;
    }

    int fileCount = mz_zip_reader_get_num_files(&zip_archive);
    if (fileCount < 1) {
        LOGS_E(getHost(), "Zip reader no files found!");
        mz_zip_reader_end(&zip_archive);
        return false;
    }

    std::filesystem::path path = getHost()->getRootPath() / std::to_string(_jobID);

    if (std::filesystem::exists(path)) {
        std::filesystem::remove_all(path);
    }

    std::filesystem::create_directory(path);

    for (int i = 0; i < fileCount; i++) {

        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
            continue;
        }

        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
            LOGS_E(getHost(), "Zip reader can not read file[%s] status!", file_stat.m_filename);
            mz_zip_reader_end(&zip_archive);
            return false;
        }

        std::filesystem::path filePath = path / file_stat.m_filename;

        zipStatus = mz_zip_reader_extract_to_file(&zip_archive, i, filePath.c_str(), 0);
        if (!zipStatus) {
            LOGS_E(getHost(), "Zip reader can not extract file[%s]!", file_stat.m_filename);
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

        auto process = getProcess(i);

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

JOB_STATUS JobItem::getStatus() {

    return status;
}
