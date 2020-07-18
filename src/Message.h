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
#define STREAM_COMPONENT 0x05

#define BLOCK_JOB_INFO 0x01
#define BLOCK_PROCESS_INFO 0x02
#define BLOCK_FILE_BINARY 0x03
#define BLOCK_FILE_MD5 0x04
#define BLOCK_FILE_INFO 0x05
#define BLOCK_COMPONENT_LIST 0x06

class Message : public BaseMessage {

private:
	MessageHeader header;
    MessageData data;

    bool readComponentList(int, std::vector<ComponentObject>&, Block*);
    bool readJobInfo(int, char*, Block*);
    bool readProcessInfo(int desc, int *, char *, Block *);
    bool readFile(int, FileItem *, const char*, long *, Block *);
    bool readFileMD5(int, Md5*, Block*);
    bool readMessageBlock(int in, Block*);

    bool writeComponentList(int, std::vector<ComponentObject>&);
    bool writeJobInfo(int, char*);
    bool writeProcessInfo(int, int, char*);
    bool writeFile(int, FileItem *, bool, bool);
    bool writeFileMD5(int, Md5*);

    bool writeMessageStream(int out);

    virtual bool readFinalize();

    virtual bool writeFinalize();

    bool deSerializeHeader(const uint8_t*);
    bool serializeHeader(uint8_t *);
    int getHeaderSize();

public:

	Message(ComponentObject host);
	Message(ComponentObject owner, MSG_TYPE type);

	MessageHeader *getHeader();
	MessageData* getData();
};

#endif //__Message_H_
