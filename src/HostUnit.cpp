//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include <string>
#include "HostUnit.h"

HostUnit::HostUnit(COMPONENT _type, TypeID _id)
        : BaseUnit(_type, ArchType::get(), _id),
        basePath(std::filesystem::current_path() / ComponentType::getName(_type)) {

    rootPath = basePath;
}

HostUnit::HostUnit(const HostUnit &copy) = default;

HostUnit::HostUnit(const HostUnit *copy)
    : BaseUnit(*copy),
    address{copy->address[COMP_DISTRIBUTOR], copy->address[COMP_COLLECTOR], copy->address[COMP_NODE]},
    basePath(copy->basePath), rootPath(copy->rootPath) {
}

HostUnit::~HostUnit() = default;

const std::filesystem::path& HostUnit::getRootPath() {

    return rootPath;
}

void HostUnit::setID(TypeID _id) {

    BaseUnit::setID(_id);

    rootPath = basePath / std::to_string(_id);

    if (std::filesystem::exists(rootPath)) {
        std::filesystem::remove_all(rootPath);
    }

    std::filesystem::create_directories(rootPath);
}

TypeAddress& HostUnit::getAddress(COMPONENT _type) {

    return address[_type];
}

void HostUnit::setAddress(COMPONENT _type, TypeAddress _address, bool isMulticast) {

    address[_type] = std::move(_address);
    address[_type]->setMulticast(isMulticast);
}

void HostUnit::setIFaceAddress(COMM_INTERFACE _type, TypeAddress _address) {

    iFaceAddress[_type] = std::move(_address);
}

TypeCommUnit HostUnit::forkCommUnit(COMPONENT _type) {

    auto unit = std::make_shared<CommUnit>(this);

    unit->setAddress(address[_type]);

    return unit;
}

TypeCommUnit HostUnit::forkCommUnit() {

    return forkCommUnit(getType());
}

TypeComponentUnit HostUnit::forkComponent(COMPONENT _type) {

    auto unit = std::make_shared<ComponentUnit>(this);

    unit->setAddress(address[_type]);

    return unit;
}

TypeComponentUnit HostUnit::forkComponent(COMM_INTERFACE _type) {

    auto unit = std::make_shared<ComponentUnit>(this);

    unit->setAddress(iFaceAddress[_type]);

    return unit;
}

TypeComponentUnit HostUnit::forkComponent() {

    return forkComponent(getType());
}
