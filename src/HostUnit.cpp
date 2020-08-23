//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "HostUnit.h"

HostUnit::HostUnit(COMPONENT _type, long _id)
        : type(_type), id(_id),
        basePath(std::filesystem::current_path() / ComponentType::getName(_type)) {

    rootPath = basePath;
}

HostUnit::HostUnit(const HostUnit &copy) = default;

HostUnit::~HostUnit() = default;

const std::filesystem::path& HostUnit::getRootPath() const {

    return rootPath;
}

long HostUnit::getID() const {

    return id;
}

void HostUnit::setID(long _id) {

    id = _id;

    rootPath = basePath / std::to_string(_id);

    if (std::filesystem::exists(rootPath)) {
        std::filesystem::remove_all(rootPath);
    }

    std::filesystem::create_directories(rootPath);
}

ARCH HostUnit::getArch() const {

    return ArchType::get();
}

COMPONENT HostUnit::getType() const {

    return type;
}

void HostUnit::setType(COMPONENT _type) {

    type = _type;
}

Address &HostUnit::getAddress(COMPONENT _out) {

    assert(_out != type);
    return address[_out];
}

void HostUnit::setAddress(COMPONENT _out, Address _address) {

    assert(_out != type);
    address[_out] = _address;
}

void HostUnit::set(COMPONENT _type, long _id, COMPONENT _out, Address _address) {

    assert(_out != _type);
    type = _type;
    id = _id;
    address[_out] = _address;
}

TypeCommUnit HostUnit::getUnit(COMPONENT targetType) {

    return std::make_shared<CommUnit>(type, getArch(), id, address[targetType]);
}
