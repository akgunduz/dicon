//
// Created by Haluk AKGUNDUZ on 23/03/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_MESSAGEHEADER_H
#define DICON_MESSAGEHEADER_H

#include "MessageType.h"
#include "HostUnit.h"

#define MAX_VARIANT 2

#define PRIORITY_COEFFICIENT 10

#define MESSAGE_DEFAULT_PRIORITY 3

class MessageHeader {

    uint16_t type{};
    uint16_t priority{MESSAGE_DEFAULT_PRIORITY};

    CommUnit owner;

    uint64_t variant[MAX_VARIANT]{};

public:
    MessageHeader();

    long getSize();

    void setType(MSG_TYPE);
    MSG_TYPE getType() const;

    TypeCommUnit getOwner();
    void setOwner(const TypeCommUnit&);
    void grabOwner(const TypeCommUnit&);

    uint64_t getVariant(int id);
    void setVariant(int id, uint64_t _variant);

    int getPriority() const;
    void setPriority(int);
    void normalizePriority();
    int iteratePriority();

    void deSerialize(const uint8_t*);
    void serialize(uint8_t*);

};

#endif //DICON_MESSAGEHEADER_H
