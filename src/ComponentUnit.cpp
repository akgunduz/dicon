//
// Created by akgun on 7.07.2020.
//

#include "ComponentUnit.h"

#include <utility>

ComponentUnit::ComponentUnit(COMPONENT _type, ARCH _arch, TypeID _id, TypeAddress _address)
    : CommUnit(_type, _arch, _id, std::move(_address)) {
}

ComponentUnit::ComponentUnit(COMPONENT _type, TypeAddress _address)
        : CommUnit(_type, std::move(_address)) {
}

ComponentUnit::ComponentUnit(COMPONENT _type)
        : CommUnit(_type) {
}

ComponentUnit::ComponentUnit(const ComponentUnit &copy) = default;

ComponentUnit::ComponentUnit(const TypeCommUnit &copy)
    : CommUnit(*copy) {
}

ComponentUnit::ComponentUnit(const CommUnit *copy)
    : CommUnit(*copy) {
}

ComponentUnit::ComponentUnit(const BaseUnit *copy)
    : CommUnit(copy) {
}

void* ComponentUnit::getHandle() const {

    return handle;
}

void ComponentUnit::setHandle(void* _handle) {

    handle = _handle;
}

TypeTime& ComponentUnit::getCheckTime() {

    return checkTime;
}

void ComponentUnit::setCheckTime() {

    checkTime = std::chrono::steady_clock::now();
}

ComponentUnit::~ComponentUnit() = default;
