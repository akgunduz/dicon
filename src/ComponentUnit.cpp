//
// Created by akgun on 7.07.2020.
//

#include "ComponentUnit.h"

ComponentUnit::ComponentUnit(COMPONENT _type, ARCH _arch, TypeID _id, Address _address)
    : CommUnit(_type, _arch, _id, _address) {
}

ComponentUnit::ComponentUnit(COMPONENT _type, Address _address)
        : CommUnit(_type, _address) {
}

ComponentUnit::ComponentUnit(COMPONENT _type)
        : CommUnit(_type) {
}

ComponentUnit::ComponentUnit(int _socket, int dump)
    : socket(_socket) {
}

ComponentUnit::ComponentUnit(const ComponentUnit &copy) = default;

ComponentUnit::ComponentUnit(const TypeCommUnit &copy)
    : CommUnit(*copy) {
}

int ComponentUnit::getSocket() const {

    return socket;
}

void ComponentUnit::setSocket(int _socket) {

    socket = _socket;
}

long long ComponentUnit::getCheckTime() const {

    return checkTime;
}

void ComponentUnit::setCheckTime(long long _checkTime) {

    checkTime = _checkTime;
}

ComponentUnit::~ComponentUnit() = default;
