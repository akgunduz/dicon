//
// Created by Haluk AKGUNDUZ on 18/10/15.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_MESSAGETYPE_H
#define DICON_MESSAGETYPE_H

#include <Common.h>

enum STREAM_TYPE {

    STREAM_ID,
    STREAM_JOBNAME,
    STREAM_COMPONENT,
    STREAM_COMPONENTS,
    STREAM_FILE_INFO,
    STREAM_FILE_BINARY,
    STREAM_PROCESS,
    STREAM_PROCESSES,
    STREAM_PROCESS_ID,
    STREAM_PROCESS_COUNT,
    STREAM_PROCESS_INFO,
    STREAM_PROCESS_FILE_INFO,
    STREAM_PROCESS_FILE_BINARY,
    STREAM_PROCESS_FILES_INFO,
    STREAM_PROCESS_FILES_BINARY,

    STREAM_NONE = 0xFFFF
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
    MSGTYPE_REPLACE,
    MSGTYPE_MAX

};

enum MSG_DIR {
    MSGDIR_RECEIVE,
    MSGDIR_SEND
};

class MessageType {

public:
    static const char* getMsgName(int);
    static const char* getMsgDirName(int);
    static void addMsg(int, std::string);
    static const char* getStreamName(int);
    static void addStream(int, std::string);
};

#endif //DICON_MESSAGETYPE_H
