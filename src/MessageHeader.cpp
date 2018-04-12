//
// Created by Haluk AKGUNDUZ on 23/03/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#include "MessageHeader.h"
#include "MessageTypes.h"

MessageHeader::MessageHeader() {

}

void MessageHeader::setType(MSG_TYPE type) {
    this->type = (int)type;
}

MSG_TYPE MessageHeader::getType() {
    return (MSG_TYPE)type;
}

COMPONENT MessageHeader::getOwner() {
    return (COMPONENT)owner;
}

void MessageHeader::setOwner(COMPONENT owner) {
    this->owner = owner;
}

long MessageHeader::getOwnerAddress() {
    return ownerAddress;
}

void MessageHeader::setOwnerAddress(long address) {
    ownerAddress = address;
}

long MessageHeader::getTime() {
    return time;
}

long MessageHeader::getDeviceID() {
    return deviceID;
}

long MessageHeader::getMessageID() {
    return messageID;
}

long MessageHeader::getVariant(int id) {

    if (id < MAX_VARIANT) {
        return variant[id];
    }

    return 0;
}

void MessageHeader::setVariant(int id, long variant) {

    if (id >= MAX_VARIANT) {
        return;
    }

    this->variant[id] = variant;
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

void MessageHeader::setPriority(int priority) {
    this->priority = priority;
}

void MessageHeader::normalizePriority() {
    priority *= PRIORITY_COEFFICIENT;
}

bool MessageHeader::set(const uint8_t* buffer) {

    type = ntohl(*((int *) buffer)); buffer += 4;
    priority = ntohl(*((int *) buffer)); buffer += 4;
    owner = ntohl(*((int *) buffer)); buffer += 4;
    ownerAddress = ntohll(*((long *) buffer)); buffer += 8;
    time = ntohll(*((long *) buffer)); buffer += 8;
    deviceID = ntohll(*((long *) buffer)); buffer += 8;
    messageID = ntohll(*((long *) buffer)); buffer += 8;

    for (int i = 0; i < MAX_VARIANT; i++) {
        variant[i] = ntohll(*((long *) buffer)); buffer += 8;
    }

    return true;
}

bool MessageHeader::extract(uint8_t *buffer) {

    *((int *) buffer) = htonl(type); buffer += 4;
    *((int *) buffer) = htonl(priority); buffer += 4;
    *((int *) buffer) = htonl(owner); buffer += 4;
    *((long *) buffer) = htonll(ownerAddress); buffer += 8;
    *((long *) buffer) = htonll(time); buffer += 8;
    *((long *) buffer) = htonll(deviceID); buffer += 8;
    *((long *) buffer) = htonll(messageID); buffer += 8;

    for (int i = 0; i < MAX_VARIANT; i++) {
        *((long *) buffer) = htonll(variant[i]); buffer += 8;
    }

    return true;
}