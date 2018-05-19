//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef BANKOR_MESSAGE_H
#define BANKOR_MESSAGE_H

#include "BaseMessage.h"
#include "MessageHeader.h"
#include "MessageData.h"

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

#endif //BANKOR_MESSAGE_H
