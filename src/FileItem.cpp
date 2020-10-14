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

    if (size[arch] == 0) {
        check(arch);
    }

    return size[arch];
}

bool FileItem::check(ARCH arch) {

    if (!is_executable) {
        arch = ARCH_NONE;
    }

    if (is_exist[arch]) {
        LOGS_T(getHost(), "FileContent %s is already validated", name.c_str());
        return true;
    }

    if (name.empty()) {
        LOGS_T(getHost(), "FileContent info is invalid");
        return false;
    }

    std::error_code ec;

    size[arch] = std::filesystem::file_size(getPath(arch), ec);

    if (size[arch] == -1) {

        //Fallback path
//        size[arch] = std::filesystem::file_size(getPath(), ec);

  //      if (size[arch] == -1) {

            LOGS_T(getHost(), "File %s Size could not read for Arch %s",
                   name.c_str(), ArchType::getName(arch));

            return false;
  //      }
    }

    if (size[arch] == 0) {
        LOGS_T(getHost(), "File %s for Arch %s is empty", name.c_str(), ArchType::getName(arch));
        return false;
    }

    is_exist[arch] = true;

    return true;
}

bool FileItem::required() const {

    return is_required;
}

void FileItem::setRequired(bool _is_required) {

    is_required = _is_required;
}

std::filesystem::path FileItem::getParentPath(ARCH arch) {

    std::filesystem::path parent = is_independent
        ? getHost()->getRootPath() / parentPath
        : getHost()->getRootPath() / std::to_string(getAssignedJob());

    return arch == ARCH_NONE || !is_executable
        ? parent
        : parent / ARCH_PATH / ArchType::getName(arch);
}

std::filesystem::path FileItem::getParentRefPath(ARCH arch) {

    std::filesystem::path parent = is_independent
        ? parentPath
        : std::filesystem::path(std::to_string(getAssignedJob()));

    return arch == ARCH_NONE || !is_executable
           ? parent
           : parent / ARCH_PATH / ArchType::getName(arch);
}

std::filesystem::path FileItem::getPath(ARCH arch) {

    return getParentPath(arch) / name;
}

std::filesystem::path FileItem::getRefPath(ARCH arch) {

    return getParentRefPath(arch) / name;
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