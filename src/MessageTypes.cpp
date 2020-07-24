//
// Created by Haluk AKGUNDUZ on 18/10/15.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "MessageTypes.h"

std::map<int, std::string> msgNameList = {
        {MSGTYPE_NODE, "NODE"},
        {MSGTYPE_JOB, "JOB"},
        {MSGTYPE_BINARY, "BINARY"},
        {MSGTYPE_INFO, "INFO"},
        {MSGTYPE_WAKEUP, "WAKEUP"},
        {MSGTYPE_READY, "READY"},
        {MSGTYPE_BUSY, "BUSY"},
        {MSGTYPE_TIMEOUT, "TIMEOUT"},
        {MSGTYPE_ALIVE, "ALIVE"},
        {MSGTYPE_ID, "ID"},
        {MSGTYPE_PROCESS, "PROCESS"},
};

std::map<int, std::string> streamNameList = {
        {STREAM_JOB, "STREAM_JOB"},
        {STREAM_COMPONENT, "STREAM_COMPONENT"},
        {STREAM_FILEINFO, "STREAM_FILEINFO"},
        {STREAM_FILEBINARY, "STREAM_FILEBINARY"},
        {STREAM_PROCESSINFO, "STREAM_PROCESSINFO"},
        {STREAM_PROCESS, "STREAM_PROCESS"},
        {STREAM_NONE, "STREAM_NONE"},
};

std::map<int, std::string> blockNameList = {
        {BLOCK_JOB, "BLOCK_JOB"},
        {BLOCK_COMPONENT, "BLOCK_COMPONENT"},
        {BLOCK_FILEINFO, "BLOCK_FILEINFO"},
        {BLOCK_FILEBINARY, "BLOCK_FILEBINARY"},
        {BLOCK_PROCESSINFO, "BLOCK_PROCESSINFO"},
        {BLOCK_PROCESS, "BLOCK_PROCESS"},
        {BLOCK_NONE, "BLOCK_NONE"},
};

const char* MessageTypes::getMsgName(int type) {

    return msgNameList[type].c_str();
}

void MessageTypes::addMsg(int type, std::string str) {

    msgNameList[type] = str;
}

const char* MessageTypes::getStreamName(int type) {

    return streamNameList[type].c_str();
}

void MessageTypes::addStream(int type, std::string str) {

    streamNameList[type] = str;
}

const char* MessageTypes::getBlockName(int type) {

    return blockNameList[type].c_str();
}

void MessageTypes::addBlock(int type, std::string str) {

    blockNameList[type] = str;
}