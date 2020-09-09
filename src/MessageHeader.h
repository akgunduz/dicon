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

#pragma pack(push, 2)
class MessageHeader {

    uint16_t type{};
    uint16_t priority{MESSAGE_DEFAULT_PRIORITY};
    uint16_t stream{};

    CommUnit owner;

public:

    MessageHeader() = default;

    long getSize();

    STREAM_TYPE getStream();
    void setStream(STREAM_TYPE stream);

    void setType(MSG_TYPE);
    MSG_TYPE getType();

    TypeCommUnit getOwner();
    void setOwner(const TypeCommUnit&);
    void grabOwner(const TypeCommUnit&);

    int getPriority();
    void setPriority(int);
    void normalizePriority();
    int iteratePriority();

    bool deSerialize(const uint8_t*);
    void serialize(uint8_t*);
};
#pragma pack(pop)

#endif //DICON_MESSAGEHEADER_H
