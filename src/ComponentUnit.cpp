//
// Created by akgun on 7.07.2020.
//

#include "ComponentUnit.h"

ComponentUnit::ComponentUnit(COMPONENT _type, long _id, Address _address)
    : CommUnit(_type, _id, _address) {
}

ComponentUnit::ComponentUnit(COMPONENT _type)
    : CommUnit(_type) {
}

ComponentUnit::ComponentUnit(const ComponentUnit &copy) = default;

ComponentUnit::ComponentUnit(const CommUnit &copy)
    : CommUnit(copy) {
}

int ComponentUnit::getSocket() {

    return socket;
}

void ComponentUnit::setSocket(int _socket) {

    socket = _socket;
}
