//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "FileItem.h"
#include "Util.h"

FileItem::FileItem(const HostUnit& host, long _id, long _assignedJob, const char* _name)
        : ContentItem (host, _id, _assignedJob) {

    strcpy(name, _name);
}

CONTENT_TYPES FileItem::getType() const {

	return CONTENT_FILE;
}

const char *FileItem::getName() const {

    return name;
}

void FileItem::setName(const char *_name) {

    strcpy(name, _name);
}

long FileItem::getSize() const {

    return size;
}

bool FileItem::check() {

    if (is_exist) {
        LOGS_T(getHost(), "FileContent %s is already validated", name);
        return true;
    }

    if (getAssignedJob() == 0 || strcmp(name, "") == 0) {
        LOGS_T(getHost(), "FileContent %s could not opened", name);
        return false;
    }

    FILE *file = fopen(Util::getAbsRefPath(getHost().getRootPath(), getAssignedJob(), name).c_str(), "r");
    if (file == nullptr) {
        LOGS_T(getHost(), "FileContent %s could not opened", name);
        return false;
    }

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    if (size == 0) {
        LOGS_T(getHost(), "FileContent %s is empty", name);
        return false;
    }

    fclose(file);

    is_exist = true;

    return true;
}

bool FileItem::required() {

    return is_required;
}

void FileItem::setRequired(bool _is_required) {

    is_required = _is_required;
}
