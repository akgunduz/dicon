//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __BaseMessage_H_
#define __BaseMessage_H_

#include "Common.h"
#include "Unit.h"
#include "Md5.h"

#define BUFFER_SIZE 512

#define PRIORITY_COEFFICIENT 10

#define SIGNATURE 0x55AA

#define STREAM_NONE 0xFFFF

#define BLOCK_END_STREAM 0xFFFF

#define BLOCK_HEADER_SIZE 8

#define TMP_BUFFER_SIZE 1000

#define MAX_VARIANT 2

#define MAX_JOBID_LENGTH 50

#define MESSAGE_DEFAULT_PRIORITY 3

//To get rid of Alignment problem stay in 64bit mod
struct MessageHeader {

    int type;
    int priority;
    int owner;
    long ownerAddress;
    long time;
    long deviceID;
    long messageID;
    long variant[MAX_VARIANT];
//	char jobID[MAX_JOBID_LENGTH];

};

class BlockHeader {

public :
    int blockType;
    int blockCount;
    int *sizes;

    BlockHeader() {

    }

    BlockHeader(int blockCount, int blockType = BLOCK_END_STREAM) {

        this->blockCount = blockCount;
        this->blockType = blockType;
        if (blockCount > 0) {
            sizes = new int[blockCount];
        }
    }

    ~BlockHeader() {

        if (blockCount > 0) {
            delete[] sizes;
        }
    }

};

class BaseMessage {

	uint8_t tmpBuf[TMP_BUFFER_SIZE];

    Unit host;

	struct MessageHeader header;

    int streamFlag;

    bool isUDP;
    sockaddr_in targetAddress;

public:

    BaseMessage(Unit host);
    BaseMessage(Unit owner, int type);

    void setStreamFlag(int);
    void setTargetAddress(long);
    void setProtocol(bool);

	int getType();

    Unit getHost();
	void setHost(Unit);
    Unit getOwner();
	void setOwner(Unit);

    long getOwnerAddress();
	void setOwnerAddress(long);
    long getVariant(int id);
	void setVariant(int id, long variant);

    long getTime();
    long getDeviceID();
    long getMessageID();

    int getPriority();
    void setPriority(int);
    void normalizePriority();
    int iteratePriority();

    int getBinarySize(const char*);
    bool transferBinary(int, int, Md5 *, int);

	bool readBlock(int, uint8_t *, int);
	bool readBinary(int, const char*, Md5 *, const char*, int);

	bool readSignature(int);
	bool readHeader(int, MessageHeader *);
	bool readBlockHeader(int, BlockHeader *);
	bool readString(int, char*, int);
	bool readNumber(int, long*);

	bool readFromStream(int);

    virtual bool readMessageBlock(int in, BlockHeader *) = 0;
    virtual bool readFinalize() = 0;

	bool writeBlock(int, const uint8_t *, int);

	bool writeSignature(int);
	bool writeHeader(int, struct MessageHeader *);
	bool writeBlockHeader(int, struct BlockHeader *);
	bool writeString(int, const char*);
	bool writeNumber(int, long);
	bool writeBinary(int, const char*, Md5 *);

	bool writeEndStream(int);

	bool writeToStream(int);

    virtual bool writeMessageStream(int out, int streamFlag) = 0;
    virtual bool writeFinalize() = 0;
};

#endif //__Message_H_
