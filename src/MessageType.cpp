//
// Created by Haluk AKGUNDUZ on 18/10/15.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "MessageType.h"

#include <utility>

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
        {MSGTYPE_REPLACE, "REPLACE"},
};

std::map<int, std::string> msgDirNameList = {
        {MSGDIR_RECEIVE, "<=="},
        {MSGDIR_SEND, "==>"},
};

std::map<int, std::string> streamNameList = {
        {STREAM_ID, "STREAM_ID"},
        {STREAM_JOBNAME, "STREAM_JOBNAME"},
        {STREAM_COMPONENT, "STREAM_COMPONENT"},
        {STREAM_COMPONENTS, "STREAM_COMPONENTS"},
        {STREAM_FILE_INFO, "STREAM_FILE_INFO"},
        {STREAM_FILE_BINARY, "STREAM_FILE_BINARY"},
        {STREAM_PROCESS, "STREAM_PROCESS"},
        {STREAM_PROCESSES, "STREAM_PROCESSES"},
        {STREAM_PROCESS_ID, "STREAM_PROCESS_ID"},
        {STREAM_PROCESS_COUNT, "STREAM_PROCESS_COUNT"},
        {STREAM_PROCESS_INFO, "STREAM_PROCESS_INFO"},
        {STREAM_PROCESS_FILE_INFO, "STREAM_PROCESS_FILE_INFO"},
        {STREAM_PROCESS_FILE_BINARY, "STREAM_PROCESS_FILE_BINARY"},
        {STREAM_PROCESS_FILES_INFO, "STREAM_PROCESS_FILES_INFO"},
        {STREAM_PROCESS_FILES_BINARY, "STREAM_PROCESS_FILES_BINARY"},
        {STREAM_NONE, "STREAM_NONE"},
};

std::map<int, std::string> blockNameList = {
        {BLOCK_JOB, "BLOCK_JOB"},
        {BLOCK_COMPONENT, "BLOCK_COMPONENT"},
        {BLOCK_FILEINFO, "BLOCK_FILEINFO"},
        {BLOCK_FILEBINARY, "BLOCK_FILEBINARY"},
        {BLOCK_PROCESSINFO, "BLOCK_PROCESSINFO"},
        {BLOCK_PROCESS, "BLOCK_PROCESS"},
        {BLOCK_END_STREAM, "BLOCK_END_STREAM"},
        {BLOCK_NONE, "BLOCK_NONE"},
};

const char* MessageType::getMsgName(int type) {

    return msgNameList[type].c_str();
}

const char* MessageType::getMsgDirName(int type) {

    return msgDirNameList[type].c_str();
}

void MessageType::addMsg(int type, std::string str) {

    msgNameList[type] = std::move(str);
}

const char* MessageType::getStreamName(int type) {

    return streamNameList[type].c_str();
}

void MessageType::addStream(int type, std::string str) {

    streamNameList[type] = std::move(str);
}

const char* MessageType::getBlockName(int type) {

    return blockNameList[type].c_str();
}

void MessageType::addBlock(int type, std::string str) {

    blockNameList[type] = std::move(str);
}