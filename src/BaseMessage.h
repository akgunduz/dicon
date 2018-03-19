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



#define BLOCK_END_STREAM 0xFFFF

#define BLOCK_HEADER_SIZE 8

#define TMP_BUFFER_SIZE 1000

#define BUSY_SLEEP_TIME 200000

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

    int headerSize;

    int desc;

    long multicastAddress;

public:

    BaseMessage(int);

    void setMulticastAddress(long);

    int getDescriptor();
    void setDescriptor(int);

    int getBinarySize(const char*);
    bool transferBinary(int, int, Md5 *, int);

	bool readBlock(int, uint8_t *, int);
	bool readBinary(int, const char*, Md5 *, int);

	bool readSignature(int);
	bool readHeader(int);
	bool readBlockHeader(int, BlockHeader *);
	bool readString(int, char*, int);
	bool readNumber(int, long*);

	bool readFromStream(int);

    virtual bool readMessageBlock(int in, BlockHeader *) = 0;
    virtual bool readFinalize() = 0;

	bool writeBlock(int, const uint8_t *, int);

	bool writeSignature(int);
	bool writeHeader(int);
	bool writeBlockHeader(int, struct BlockHeader *);
	bool writeString(int, const char*);
	bool writeNumber(int, long);
	bool writeBinary(int, const char*, Md5 *, int);

	bool writeEndStream(int);

	bool writeToStream(int);

    virtual bool writeMessageStream(int out) = 0;
    virtual bool writeFinalize() = 0;

    virtual bool parseHeader(const uint8_t*) = 0;
    virtual bool prepareHeader(uint8_t *) = 0;
};

#endif //__Message_H_
