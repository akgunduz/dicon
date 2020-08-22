//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "FileItem.h"

#include <utility>
#include "Util.h"

FileItem::FileItem(const TypeHostUnit& host, long _id, long _assignedJob, std::string _name)
        : ContentItem (host, _id, _assignedJob),  name(std::move(_name)) {
}

CONTENT_TYPES FileItem::getType() const {

	return CONTENT_FILE;
}

const std::string& FileItem::getName() const {

    return name;
}

void FileItem::setName(const std::string& _name) {

    name = _name;
}

long FileItem::getSize() const {

    return size;
}

bool FileItem::check() {

    if (is_exist) {
        LOGS_T(getHost(), "FileContent %s is already validated", name.c_str());
        return true;
    }

    if (getAssignedJob() == 0 || name.empty()) {
        LOGS_T(getHost(), "FileContent info is invalid");
        return false;
    }

    try {

        size = std::filesystem::file_size(getHost()->getRootPath() / std::to_string(getAssignedJob()) / name);

    } catch(std::filesystem::filesystem_error& e) {

        LOGS_T(getHost(), "FileContent %s could not opened, err : %s", name.c_str(), e.what());
        return false;
    }

    if (size == 0) {
        LOGS_T(getHost(), "FileContent %s is empty", name.c_str());
        return false;
    }

    is_exist = true;

    return true;
}

bool FileItem::required() const {

    return is_required;
}

void FileItem::setRequired(bool _is_required) {

    is_required = _is_required;
}
