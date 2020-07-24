//
// Created by Haluk AKGUNDUZ on 23/03/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_MESSAGEHEADER_H
#define DICON_MESSAGEHEADER_H

#include "MessageTypes.h"
#include "ComponentObject.h"

#define MAX_VARIANT 2

#define PRIORITY_COEFFICIENT 10

#define MESSAGE_DEFAULT_PRIORITY 3

class MessageHeader {

    int type{};
    int priority{};

    long owner{};
    long ownerAddress{};
    long variant[MAX_VARIANT]{};

public:
    MessageHeader();

    long getSize();

    void setType(MSG_TYPE);
    MSG_TYPE getType() const;

    ComponentObject getOwner() const;
    void setOwner(const ComponentObject&, COMPONENT = COMP_DISTRIBUTOR);

    long getVariant(int id);
    void setVariant(int id, long _variant);

    int getPriority() const;
    void setPriority(int);
    void normalizePriority();
    int iteratePriority();

    bool deSerialize(const uint8_t*);
    bool serialize(uint8_t*);

};


#endif //DICON_MESSAGEHEADER_H
