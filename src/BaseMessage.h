//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __BaseMessage_H_
#define __BaseMessage_H_

#include "Md5.h"
#include "Block.h"

#define BUFFER_SIZE 512

#define SIGNATURE 0x55AA

#define TMP_BUFFER_SIZE 1000

#define BUSY_SLEEP_TIME 200000

class BaseMessage {

	uint8_t tmpBuf[TMP_BUFFER_SIZE];

    int headerSize;

    int desc;

	sockaddr_in multicastAddress;

public:

    BaseMessage(int);

    void setMulticastAddress(sockaddr_in);

    int getDescriptor();
    void setDescriptor(int);

    int getBinarySize(const char*);
    bool transferBinary(int, int, Md5 *, int);

	bool readBlock(int, uint8_t *, int);

	bool readSignature(int);
	bool readHeader(int);
	bool readBlockHeader(int, Block*);
	bool readString(int, char*, int);
	bool readNumber(int, long*);
	bool readMD5(int, Md5*);
    bool readBinary(int, const char*, Md5 *, int);

	bool readFromStream(int);

    virtual bool readMessageBlock(int in, Block*) = 0;
    virtual bool readFinalize() = 0;

	bool writeBlock(int, const uint8_t *, int);

	bool writeSignature(int);
	bool writeHeader(int);
	bool writeBlockHeader(int, struct Block*);
	bool writeString(int, const char*);
	bool writeNumber(int, long);
    bool writeMD5(int, Md5*);
	bool writeBinary(int, const char*, Md5 *, int);

	bool writeToStream(int);

    bool writeEndStream(int);

    virtual bool writeMessageStream(int out) = 0;
    virtual bool writeFinalize() = 0;

    virtual bool setHeader(const uint8_t*) = 0;
    virtual bool extractHeader(uint8_t *) = 0;
};

#endif //__Message_H_
