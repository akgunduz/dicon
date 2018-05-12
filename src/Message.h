//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Message_H_
#define __Message_H_

#include "BaseMessage.h"
#include "MessageHeader.h"
#include "MessageData.h"

//#define BLOCK_JOB_ID 0x01
//#define BLOCK_JOB_INFO 0x02
//#define BLOCK_EXECUTION_INFO 0x03
//#define BLOCK_FILE_BINARY 0x04
//#define BLOCK_FILE_MD5 0x05
//#define BLOCK_FILE_INFO 0x06

class Message : public BaseMessage {

private:
	MessageHeader header;
    MessageData data;

    bool readJobID(int, Uuid*, struct Block*);
    bool readJobDir(int, char*, struct Block*);
    bool readProcessID(int, long*, struct Block*);
    bool readProcessCommand(int, char*, struct Block*);
    bool readFile(int, FileItem *, const char*, long *, struct Block *);
    bool readFileMD5(int, Md5*, struct Block*);
    bool readMessageBlock(int in, Block*);

    bool writeJobID(int, Uuid);
    bool writeJobDir(int, char*);
    //bool writeJobInfo(int, Uuid, char*);
    bool writeProcessID(int, long);
    bool writeProcessCommand(int, char*);
    bool writeFile(int, FileItem *, bool, bool);
    bool writeFileMD5(int, Md5*);

    bool writeMessageStream(int out);

    virtual bool readFinalize();

    virtual bool writeFinalize();

    bool setHeader(const uint8_t*);
    bool extractHeader(uint8_t *);
    int getHeaderSize();

public:

	Message(ComponentObject host);
	Message(ComponentObject owner, MSG_TYPE type);

	MessageHeader *getHeader();
	MessageData* getData();
};

#endif //__Message_H_
