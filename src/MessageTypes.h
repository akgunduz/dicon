//
// Created by Haluk AKGUNDUZ on 18/10/15.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_MESSAGETYPES_H
#define DICON_MESSAGETYPES_H

#include <Common.h>

enum STREAM_TYPE {

    STREAM_JOB,
    STREAM_COMPONENT,
    STREAM_FILEINFO,
    STREAM_FILEBINARY,
    STREAM_PROCESSINFO,
    STREAM_PROCESS,

    STREAM_NONE = 0xFFFF
};

enum BLOCK_TYPE {

    BLOCK_JOB,
    BLOCK_COMPONENT,
    BLOCK_FILEINFO,
    BLOCK_FILEBINARY,
    BLOCK_PROCESSINFO,
    BLOCK_PROCESSID,
    BLOCK_PROCESS,
    BLOCK_END_STREAM,

    BLOCK_NONE = 0xFFFF
};

enum MSG_TYPE {
    MSGTYPE_NODE,
    MSGTYPE_JOB,
    MSGTYPE_BINARY,
    MSGTYPE_INFO,
    MSGTYPE_WAKEUP,
    MSGTYPE_READY,
    MSGTYPE_BUSY,
    MSGTYPE_TIMEOUT,
    MSGTYPE_ALIVE,
    MSGTYPE_ID,
    MSGTYPE_PROCESS,
    MSGTYPE_MAX

};

enum MSG_DIR {
    MSGDIR_RECEIVE,
    MSGDIR_SEND
};

class MessageTypes {

public:
    static const char* getMsgName(int);
    static const char* getMsgDirName(int);
    static void addMsg(int, std::string);
    static const char* getStreamName(int);
    static void addStream(int, std::string);
    static const char* getBlockName(int);
    static void addBlock(int, std::string);
};

#endif //DICON_MESSAGETYPES_H
