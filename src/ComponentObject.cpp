//
// Created by Haluk AKGUNDUZ on 01.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "ComponentObject.h"

TypeComponentUnit& ComponentObject::getAssigned() {

    return assigned;
}

void ComponentObject::setAssigned(COMPONENT _type, ARCH _arch, long _id, TypeAddress _address) {

    assigned = std::make_shared<ComponentUnit>(_type, _arch, _id, _address);
}

void ComponentObject::setAssigned(const TypeComponentUnit& unit) {

    assigned = unit;
}
