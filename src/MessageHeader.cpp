//
// Created by Haluk AKGUNDUZ on 23/03/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#include "MessageHeader.h"
#include "MessageTypes.h"

MessageHeader::MessageHeader() = default;

long MessageHeader::getSize() {

    return sizeof(MessageHeader);
}

void MessageHeader::setType(MSG_TYPE _type) {

    type = (int)_type;
}

MSG_TYPE MessageHeader::getType() const {

    return (MSG_TYPE)type;
}

ComponentObject MessageHeader::getOwner() const {

    return ComponentObject((COMPONENT)(owner >> 32), (int)(owner & 0xFFFFFFFF), ownerAddress);
}

void MessageHeader::setOwner(const ComponentObject& _owner, COMPONENT targetInterface) {

    this->owner = (((long)_owner.getType()) << 32) | _owner.getID();
    this->ownerAddress = _owner.getAddress(targetInterface);
}

long MessageHeader::getVariant(int id) {

    if (id < MAX_VARIANT) {
        return variant[id];
    }

    return 0;
}

void MessageHeader::setVariant(int id, long _variant) {

    if (id >= MAX_VARIANT) {
        return;
    }

    this->variant[id] = _variant;
}

int MessageHeader::getPriority() const {

    return priority;
}

int MessageHeader::iteratePriority() {

    if (priority > 1) {
        priority--;
    }

    return priority;
}

void MessageHeader::setPriority(int _priority) {

    priority = _priority;
}

void MessageHeader::normalizePriority() {

    priority *= PRIORITY_COEFFICIENT;
}

bool MessageHeader::deSerialize(const uint8_t* buffer) {

    type = ntohl(*((int *) buffer)); buffer += 4;
    priority = ntohl(*((int *) buffer)); buffer += 4;
    owner = ntohll(*((long *) buffer)); buffer += 8;
    ownerAddress = ntohll(*((long *) buffer)); buffer += 8;
    for (int i = 0; i < MAX_VARIANT; i++) {
        variant[i] = ntohll(*((long *) buffer)); buffer += 8;
    }

    return true;
}

bool MessageHeader::serialize(uint8_t *buffer) {

    *((int *) buffer) = htonl(type); buffer += 4;
    *((int *) buffer) = htonl(priority); buffer += 4;
    *((long *) buffer) = htonll(owner); buffer += 8;
    *((long *) buffer) = htonll(ownerAddress); buffer += 8;
    for (int i = 0; i < MAX_VARIANT; i++) {
        *((long *) buffer) = htonll(variant[i]); buffer += 8;
    }

    return true;
}
