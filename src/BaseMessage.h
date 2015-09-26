//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __BaseMessage_H_
#define __BaseMessage_H_

#include "Common.h"

#define BUFFER_SIZE 512

#define PRIORITY_COEFFICIENT 10

#define DEFAULT_PRIORITY PRIORITY_3

#define SIGNATURE 0x55AA

#define STREAM_NONE 0xFFFF

#define BLOCK_END_STREAM 0xFFFF

#define BLOCK_HEADER_SIZE 8

#define TMP_BUFFER_SIZE 1000

#define MAX_VARIANT 2

enum PRIORITIES {
    PRIORITY_HEAD,
    PRIORITY_1,
    PRIORITY_2,
    PRIORITY_3,
    PRIORITY_4,
    PRIORITY_5,
    PRIORITY_6,
    PRIORITY_7,
    PRIORITY_8,
    PRIORITY_9,
    PRIORITY_MAX,
};

//To get rid of Alignment problem stay in 64bit mod
struct MessageHeader {

    int type;
    int priority;
    long ownerAddress;
    long time;
    long deviceID;
    long messageID;
    long variant[MAX_VARIANT];

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

	struct MessageHeader header;

    int streamFlag;

public:

    BaseMessage();
    BaseMessage(long time, long deviceID, int type = 0);

    void setStreamFlag(int);

	int getType();

    long getOwnerAddress();
	void setOwnerAddress(long);
    long getVariant(int id);
	void setVariant(int id, long variant);

    long getTime();
    long getDeviceID();
    long getMessageID();

    PRIORITIES getPriority();
    void setPriority(PRIORITIES);
    void normalizePriority();
    PRIORITIES iteratePriority();

    int getBinarySize(const std::string&);
    bool transferBinary(int, int, uint8_t *, int);

	bool readBlock(int, uint8_t *, int);
	bool readBinary(int, const std::string&, uint8_t *, int);

	bool readSignature(int);
	bool readHeader(int, MessageHeader *);
	bool readBlockHeader(int, BlockHeader *);
	bool readString(int, std::string&, int);
	bool readNumber(int, long*);

	bool readFromStream(int);

    virtual bool readMessageBlock(int in, BlockHeader *) = 0;

	bool writeBlock(int, const uint8_t *, int);

	bool writeSignature(int);
	bool writeHeader(int, struct MessageHeader *);
	bool writeBlockHeader(int, struct BlockHeader *);
	bool writeString(int, const std::string&);
	bool writeNumber(int, long);
	bool writeBinary(int, const std::string&, uint8_t *);

	bool writeEndStream(int);

	bool writeToStream(int);

    virtual bool writeMessageStream(int out, int streamFlag) = 0;
};

#endif //__Message_H_
