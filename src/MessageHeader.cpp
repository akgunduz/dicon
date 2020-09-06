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

void MessageHeader::setOwner(const TypeCommUnit& _owner) {

    owner = *_owner;
}

void MessageHeader::grabOwner(const TypeCommUnit& unit) {

    unit->set(owner);
}

//uint64_t MessageHeader::getVariant(int id) {
//
//    if (id < MAX_VARIANT) {
//        return variant[id];
//    }
//
//    return 0;
//}
//
//void MessageHeader::setVariant(int id, uint64_t _variant) {
//
//    if (id >= MAX_VARIANT) {
//        return;
//    }
//
//    this->variant[id] = _variant;
//}

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
    owner.type = ntohs(*((uint16_t *) buffer)); buffer += 2;
    owner.arch = ntohs(*((uint16_t *) buffer)); buffer += 2;
    owner.id = ntohl(*((TypeID *) buffer)); buffer += sizeof(TypeID);
    owner.address.flag = ntohs(*((uint16_t *) buffer)); buffer += 2;
    owner.address.get().port = ntohs(*((uint16_t *) buffer)); buffer += 2;
    owner.address.get().base = ntohl(*((uint32_t *) buffer)); buffer += 4;
    owner.address.getUI().port = ntohs(*((uint16_t *) buffer)); buffer += 2;
    owner.address.getUI().base = ntohl(*((uint32_t *) buffer)); buffer += 4;
//    for (int i = 0; i < MAX_VARIANT; i++) {
//        variant[i] = ntohll(*((uint64_t *) buffer)); buffer += 8;

    return true;
}

void MessageHeader::serialize(uint8_t *buffer) {

    *((uint16_t *) buffer) = htons(type); buffer += 2;
    *((uint16_t *) buffer) = htons(priority); buffer += 2;
    *((uint16_t *) buffer) = htons(stream); buffer += 2;
    *((uint16_t *) buffer) = htons(owner.type); buffer += 2;
    *((uint16_t *) buffer) = htons(owner.arch); buffer += 2;
    *((TypeID *) buffer) = htonl(owner.id); buffer += sizeof(TypeID);
    *((uint16_t *) buffer) = htons(owner.address.flag); buffer += 2;
    *((uint16_t *) buffer) = htons(owner.address.get().port); buffer += 2;
    *((uint32_t *) buffer) = htonl(owner.address.get().base); buffer += 4;
    *((uint16_t *) buffer) = htons(owner.address.getUI().port); buffer += 2;
    *((uint32_t *) buffer) = htonl(owner.address.getUI().base); buffer += 4;
//    for (int i = 0; i < MAX_VARIANT; i++) {
//        *((uint64_t *) buffer) = htonll(variant[i]); buffer += 8;
//    }
}
