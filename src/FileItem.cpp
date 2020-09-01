//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "FileItem.h"

#include <utility>
#include "Util.h"

FileItem::FileItem(const TypeHostUnit& host, long _id, long _assignedJob, std::string _name)
        : ContentItem (host, _id, _assignedJob),
          name(std::move(_name)),
          parentPath("."),
          is_independent(false) {
}

FileItem::FileItem(const TypeHostUnit& host, bool _is_independent)
        : ContentItem (host, 0, 0), parentPath("."), is_independent(_is_independent) {
}

FileItem::FileItem(const TypeHostUnit& host, long _id, std::filesystem::path _path, std::string _name)
        : ContentItem (host, _id, 0),
          name(std::move(_name)),
          parentPath(std::move(_path)),
          is_independent(true) {
}

CONTENT_TYPES FileItem::getType() {

	return CONTENT_FILE;
}

const std::string& FileItem::getName() {

    return name;
}

void FileItem::setName(const std::string& _name) {

    name = _name;
}

std::uintmax_t FileItem::getSize() {

    if (size == 0) {
        check();
    }

    return size;
}

bool FileItem::check() {

    if (is_exist) {
        LOGS_T(getHost(), "FileContent %s is already validated", name.c_str());
        return true;
    }

    if (name.empty()) {
        LOGS_T(getHost(), "FileContent info is invalid");
        return false;
    }

    try {

        size = std::filesystem::file_size(getPath());

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

std::filesystem::path FileItem::getParentPath() {

    return is_independent
        ? getHost()->getRootPath() / parentPath
        : getHost()->getRootPath() / std::to_string(getAssignedJob());
}

std::filesystem::path FileItem::getParentRefPath() {

    return is_independent
        ? parentPath
        : std::filesystem::path(std::to_string(getAssignedJob()));
}

std::filesystem::path FileItem::getPath() {

    return getParentPath() / name;
}

std::filesystem::path FileItem::getRefPath() {

    return getParentRefPath() / name;
}

bool FileItem::isIndependent() const {

    return is_independent;
}

void FileItem::setIndependent(bool _is_independent) {

    is_independent = _is_independent;
}
