//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#include "ComponentObject.h"

ComponentUnit &ComponentObject::getAssigned() {

    return assigned;
}

void ComponentObject::setAssigned(COMPONENT _type, long _id, Address _address) {

    assigned.setType(_type);
    assigned.setID(_id);
    assigned.setAddress(_address);
}
