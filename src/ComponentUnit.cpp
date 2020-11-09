//
// Created by Haluk AKGUNDUZ on 07.07.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "ComponentUnit.h"

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

TypeTimeDuration ComponentUnit::getDuration() {

    return timer.duration();
}

void ComponentUnit::resetTimer() {

    timer.reset();
}

ComponentUnit::~ComponentUnit() = default;
