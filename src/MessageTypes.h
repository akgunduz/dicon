//
// Created by Haluk AKGUNDUZ on 18/10/15.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#ifndef BANKOR_MESSAGETYPES_H
#define BANKOR_MESSAGETYPES_H

enum MSG_TYPE {
    MSGTYPE_NODE,
    MSGTYPE_JOB,
    MSGTYPE_BINARY,
    MSGTYPE_MD5,
    MSGTYPE_INFO,
    MSGTYPE_WAKEUP,
    MSGTYPE_READY,
    MSGTYPE_BUSY,
    MSGTYPE_TIMEOUT,
    MSGTYPE_ALIVE,
    MSGTYPE_MAX

};

class MessageTypes {

public:
    static const char* getName(MSG_TYPE);
};


#endif //BANKOR_MESSAGETYPES_H
