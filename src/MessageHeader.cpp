//
// Created by Haluk AKGUNDUZ on 23/03/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "MessageHeader.h"

long MessageHeader::getSize() {

    return sizeof(MessageHeader);
}

void MessageHeader::setType(MSG_TYPE _type) {

    type = (uint16_t)_type;
}

MSG_TYPE MessageHeader::getType() {

    return (MSG_TYPE)type;
}

STREAM_TYPE MessageHeader::getStream() {

    return (STREAM_TYPE)stream;
}

void MessageHeader::setStream(STREAM_TYPE _stream) {

    stream = (uint16_t)_stream;
}

TypeCommUnit MessageHeader::getOwner() {

    return std::make_shared<CommUnit>(owner);
}

void MessageHeader::setOwner(const CommUnit& _owner) {

    owner = _owner;
}

void MessageHeader::grabOwner(const TypeCommUnit& unit) {

    unit->set(owner);
}

int MessageHeader::getPriority() {

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

    type = ntohs(*((uint16_t *) buffer)); buffer += 2;
    priority = ntohs(*((uint16_t *) buffer)); buffer += 2;
    stream = ntohs(*((uint16_t *) buffer)); buffer += 2;
    owner.deSerialize(buffer);

    return true;
}

void MessageHeader::serialize(uint8_t *buffer) {

    *((uint16_t *) buffer) = htons(type); buffer += 2;
    *((uint16_t *) buffer) = htons(priority); buffer += 2;
    *((uint16_t *) buffer) = htons(stream); buffer += 2;
    owner.serialize(buffer);

}
