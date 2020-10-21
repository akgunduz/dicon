//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "FileItem.h"
#include "Log.h"

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

std::uintmax_t FileItem::getSize(ARCH arch) {

    return size[arch] == 0 ? size[ARCH_FREE] : size[arch];
}

bool FileItem::checkContent() {

    if (is_valid) {
        return true;
    }

    if (name.empty()) {
        LOGS_T(getHost(), "FileContent info is invalid");
        return false;
    }

    std::error_code ec;

    size[ARCH_FREE] = std::filesystem::file_size(getPath(), ec);

    size[ARCH_FREE] == -1 ? size[ARCH_FREE] = 0 : is_valid = true;

    if (is_executable) {

        for (int i = ARCH_FREE + 1; i < ARCH_MAX; i++) {

            size[i] = std::filesystem::file_size(getPath(static_cast<ARCH>(i), false), ec);

            size[i] == -1 ? size[i] = 0 : is_valid = true;
        }
    }

    if (!is_valid) {
        LOGS_T(getHost(), "FileContent %s could not opened or empty", name.c_str());
        return false;
    }

    return true;
}

bool FileItem::required() const {

    return is_required;
}

void FileItem::setRequired(bool _is_required) {

    is_required = _is_required;
}

std::filesystem::path FileItem::getParentRefPath(ARCH arch, bool forwardDefault) {

    std::filesystem::path parent = is_independent
        ? parentPath
        : std::filesystem::path(std::to_string(getAssignedJob()));

    return arch == ARCH_FREE || (forwardDefault && size[arch] == 0)
           ? parent
           : parent / ARCH_PATH / ArchType::getName(arch);
}

std::filesystem::path FileItem::getRefPath(ARCH arch, bool forwardDefault) {

    return getParentRefPath(arch, forwardDefault) / name;
}

std::filesystem::path FileItem::getParentPath(ARCH arch, bool forwardDefault) {

    return !is_independent ? getHost()->getRootPath() / getParentRefPath(arch, forwardDefault)
        : getParentRefPath(arch, forwardDefault);
}

std::filesystem::path FileItem::getPath(ARCH arch, bool forwardDefault) {

    return getParentPath(arch, forwardDefault) / name;
}

bool FileItem::isIndependent() const {

    return is_independent;
}

void FileItem::setIndependent(bool _is_independent) {

    is_independent = _is_independent;
}

bool FileItem::isExecutable() const {

    return is_executable;
}

void FileItem::setExecutable(bool _is_executable) {

    is_executable = _is_executable;
}