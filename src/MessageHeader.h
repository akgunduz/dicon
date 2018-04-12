//
// Created by Haluk AKGUNDUZ on 23/03/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#ifndef BANKOR_MESSAGEHEADER_H
#define BANKOR_MESSAGEHEADER_H

#include "MessageTypes.h"
#include "ComponentTypes.h"

#define MAX_VARIANT 2

#define PRIORITY_COEFFICIENT 10

#define MESSAGE_DEFAULT_PRIORITY 3

class MessageHeader {

    int type;
    int priority;
    int owner;
    long ownerAddress;
    long time;
    long deviceID;
    long messageID;
    long variant[MAX_VARIANT];

public:
    MessageHeader();
    void setType(MSG_TYPE);
    MSG_TYPE getType();
    COMPONENT getOwner();
    void setOwner(COMPONENT);

    long getOwnerAddress();
    void setOwnerAddress(long);
    long getVariant(int id);
    void setVariant(int id, long variant);

    long getTime();
    long getDeviceID();
    long getMessageID();

    int getPriority();
    void setPriority(int);
    void normalizePriority();
    int iteratePriority();

    bool set(const uint8_t*);
    bool extract(uint8_t*);

};


#endif //BANKOR_MESSAGEHEADER_H
