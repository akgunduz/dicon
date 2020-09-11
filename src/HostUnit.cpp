//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "HostUnit.h"

HostUnit::HostUnit(COMPONENT _type, TypeID _id)
        : CommUnit(_type, ArchType::get(), _id),
        basePath(std::filesystem::current_path() / ComponentType::getName(_type)) {

    rootPath = basePath;
}

HostUnit::~HostUnit() = default;

const std::filesystem::path& HostUnit::getRootPath() {

    return rootPath;
}

void HostUnit::setID(TypeID _id) {

    CommUnit::setID(_id);

    rootPath = basePath / std::to_string(_id);

    if (std::filesystem::exists(rootPath)) {
        std::filesystem::remove_all(rootPath);
    }

    std::filesystem::create_directories(rootPath);
}
