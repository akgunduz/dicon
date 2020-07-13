//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __BaseMessage_H_
#define __BaseMessage_H_

#include "Md5.h"
#include "Block.h"
#include "ComponentObject.h"

#define SIGNATURE 0x55AA
#define SIGNATURE_SIZE 2

#define TMP_BUFFER_SIZE 1000

#define BUSY_SLEEP_TIME 200

class BaseMessage {

	uint8_t tmpBuf[TMP_BUFFER_SIZE];

	ComponentObject host;

	sockaddr_in datagramAddress;

public:

    BaseMessage(ComponentObject&);

    void setDatagramAddress(sockaddr_in);

    bool transferBinary(int, int, Md5 *, long);

	bool readBlock(int, uint8_t *, long);

	bool readSignature(int);
	bool readHeader(int);
	bool readBlockHeader(int, Block*);
	bool readString(int, char*, int);
	bool readNumber(int, long*);
    bool readNumberList(int, std::vector<long> &, int);
	bool readMD5(int, Md5*);
    bool readBinary(int, const char*, Md5 *, long);

	bool readFromStream(int);

    virtual bool readMessageBlock(int in, Block*) = 0;
    virtual bool readFinalize() = 0;

	bool writeBlock(int, const uint8_t *, long);

	bool writeSignature(int);
	bool writeHeader(int);
	bool writeBlockHeader(int, Block*);
	bool writeString(int, const char*);
	bool writeNumber(int, long);
    bool writeNumberList(int, std::vector<long>&);
    bool writeMD5(int, Md5*);
	bool writeBinary(int, const char*, Md5 *, long);

	bool writeToStream(int);

    bool writeEndStream(int);

    virtual bool writeMessageStream(int out) = 0;
    virtual bool writeFinalize() = 0;

    virtual bool deSerializeHeader(const uint8_t*) = 0;
    virtual bool serializeHeader(uint8_t *) = 0;
    virtual int getHeaderSize() = 0;

	ComponentObject getHost();
};

#endif //__Message_H_
