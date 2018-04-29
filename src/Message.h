//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Message_H_
#define __Message_H_

#include "BaseMessage.h"
#include "MessageHeader.h"
#include "MessageData.h"

#define STREAM_NONE 0xFFFF

#define STREAM_INFO 0x01
#define STREAM_BINARY 0x02
#define STREAM_MD5 0x03
#define STREAM_JOB 0x04

#define BLOCK_JOB_INFO 0x01
#define BLOCK_EXECUTION_INFO 0x02
#define BLOCK_FILE_BINARY 0x03
#define BLOCK_FILE_MD5 0x04
#define BLOCK_FILE_INFO 0x05

class Message : public BaseMessage {

private:
	MessageHeader header;
    MessageData data;

    bool readJobInfo(int, char*, struct Block*);
    bool readExecutionInfo(int, long*, char*, struct Block*);
    bool readFile(int, FileItem *, const char*, long *, struct Block *);
    bool readFileMD5(int, Md5*, struct Block*);
    bool readMessageBlock(int in, Block*);

    bool writeJobInfo(int, char*);
    bool writeExecutionInfo(int, long, char*);
    bool writeFile(int, FileItem *, bool, bool);
    bool writeFileMD5(int, Md5*);

    bool writeMessageStream(int out);

    virtual bool readFinalize();

    virtual bool writeFinalize();

    bool setHeader(const uint8_t*);
    bool extractHeader(uint8_t *);
    int getHeaderSize();

public:

	Message(COMPONENT host, int id);
	Message(COMPONENT owner, int id, MSG_TYPE type);

	MessageHeader *getHeader();
	MessageData* getData();
};

#endif //__Message_H_
