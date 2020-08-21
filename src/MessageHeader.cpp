//
// Created by Haluk AKGUNDUZ on 23/03/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#include "MessageHeader.h"
#include "MessageType.h"

MessageHeader::MessageHeader() = default;

long MessageHeader::getSize() {

    return sizeof(MessageHeader);
}

void MessageHeader::setType(MSG_TYPE _type) {

    type = (uint16_t)_type;
}

MSG_TYPE MessageHeader::getType() const {

    return (MSG_TYPE)type;
}

CommUnit& MessageHeader::getOwner() {

    return owner;
}

void MessageHeader::setOwner(CommUnit _owner) {

    owner = _owner;
}

void MessageHeader::grabOwner(CommUnit &unit) {

    unit.grab(owner);
}

uint64_t MessageHeader::getVariant(int id) {

    if (id < MAX_VARIANT) {
        return variant[id];
    }

    return 0;
}

void MessageHeader::setVariant(int id, uint64_t _variant) {

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

void MessageHeader::deSerialize(const uint8_t* buffer) {

    type = ntohs(*((uint16_t *) buffer)); buffer += 2;
    priority = ntohs(*((uint16_t *) buffer)); buffer += 2;
    owner.type = ntohs(*((uint16_t *) buffer)); buffer += 2;
    owner.arch = ntohs(*((uint16_t *) buffer)); buffer += 2;
    owner.id = ntohl(*((uint32_t *) buffer)); buffer += 4;
    owner.address.flag = ntohs(*((uint16_t *) buffer)); buffer += 2;
    owner.address.get().port = ntohs(*((uint16_t *) buffer)); buffer += 2;
    owner.address.get().base = ntohl(*((uint32_t *) buffer)); buffer += 4;
    owner.address.getUI().port = ntohs(*((uint16_t *) buffer)); buffer += 2;
    owner.address.getUI().base = ntohl(*((uint32_t *) buffer)); buffer += 4;
    for (int i = 0; i < MAX_VARIANT; i++) {
        variant[i] = ntohll(*((uint64_t *) buffer)); buffer += 8;
    }
}

void MessageHeader::serialize(uint8_t *buffer) {

    *((uint16_t *) buffer) = htons(type); buffer += 2;
    *((uint16_t *) buffer) = htons(priority); buffer += 2;
    *((uint16_t *) buffer) = htons(owner.type); buffer += 2;
    *((uint16_t *) buffer) = htons(owner.arch); buffer += 2;
    *((uint32_t *) buffer) = htonl(owner.id); buffer += 4;
    *((uint16_t *) buffer) = htons(owner.address.flag); buffer += 2;
    *((uint16_t *) buffer) = htons(owner.address.get().port); buffer += 2;
    *((uint32_t *) buffer) = htonl(owner.address.get().base); buffer += 4;
    *((uint16_t *) buffer) = htons(owner.address.getUI().port); buffer += 2;
    *((uint32_t *) buffer) = htonl(owner.address.getUI().base); buffer += 4;
    for (int i = 0; i < MAX_VARIANT; i++) {
        *((uint64_t *) buffer) = htonll(variant[i]); buffer += 8;
    }
}
