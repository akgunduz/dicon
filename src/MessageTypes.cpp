//
// Created by Haluk AKGUNDUZ on 18/10/15.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#include "MessageTypes.h"

const char* sMsgTypes[MSGTYPE_MAX] = {
        "NODE",
        "RULE",
        "BINARY",
        "MD5",
        "WAKEUP",
        "READY",
        "BUSY",
        "TIMEOUT",
        "ALIVE",
};

const char* MessageTypes::getName(MSG_TYPE type) {

    return sMsgTypes[type];
}
