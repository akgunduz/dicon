//
// Created by Haluk AKGUNDUZ on 03.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "CommUnit.h"
#include "Platform.h"

CommUnit::CommUnit(COMPONENT _type, ARCH _arch, TypeID _id, TypeAddress _address)
        : BaseUnit(_type, _arch, _id), address(std::move(_address)) {
}

CommUnit::CommUnit(COMPONENT _type, ARCH _arch, TypeID _id)
        : BaseUnit(_type, _arch, _id) {

    address = std::make_shared<Address>();
}

CommUnit::CommUnit(COMPONENT _type, TypeAddress _address)
        : BaseUnit(_type), address(std::move(_address)) {
}

CommUnit::CommUnit(COMPONENT _type)
        : BaseUnit(_type) {

    address = std::make_shared<Address>();
}

CommUnit::CommUnit(const BaseUnit *copy)
        : BaseUnit(*copy) {
}

CommUnit::~CommUnit() = default;

void CommUnit::setID(TypeID id) {

    BaseUnit::setID(id);
}

TypeAddress& CommUnit::getAddress() {

    return address;
}

void CommUnit::setAddress(const TypeAddress& _address, bool isMulticast) {

    address = _address;
    address->setMulticast(isMulticast);
}

void CommUnit::set(const CommUnit &unit) {

    *this = unit;
}

bool CommUnit::deSerialize(const uint8_t* buffer) {

    type = ntohs(*((uint16_t *) buffer)); buffer += 2;
    arch = ntohs(*((uint16_t *) buffer)); buffer += 2;
    id = ntohl(*((TypeID *) buffer)); buffer += sizeof(TypeID);
    address->getFlag() = ntohs(*((uint16_t *) buffer)); buffer += 2;
    address->get().port = ntohs(*((uint16_t *) buffer)); buffer += 2;
    address->get().base = ntohl(*((uint32_t *) buffer)); buffer += 4;
    address->getUI().port = ntohs(*((uint16_t *) buffer)); buffer += 2;
    address->getUI().base = ntohl(*((uint32_t *) buffer)); buffer += 4;

    return true;
}

void CommUnit::serialize(uint8_t *buffer) {

    *((uint16_t *) buffer) = htons(type); buffer += 2;
    *((uint16_t *) buffer) = htons(arch); buffer += 2;
    *((TypeID *) buffer) = htonl(id); buffer += sizeof(TypeID);
    *((uint16_t *) buffer) = htons(address->getFlag()); buffer += 2;
    *((uint16_t *) buffer) = htons(address->get().port); buffer += 2;
    *((uint32_t *) buffer) = htonl(address->get().base); buffer += 4;
    *((uint16_t *) buffer) = htons(address->getUI().port); buffer += 2;
    *((uint32_t *) buffer) = htonl(address->getUI().base); buffer += 4;

}
