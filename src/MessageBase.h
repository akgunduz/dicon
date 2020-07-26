//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_MESSAGEBASE_H
#define DICON_MESSAGEBASE_H

#include "crc/CRC.h"
#include "MessageBlockHeader.h"
#include "MessageTypes.h"
#include "ComponentObject.h"

#define SIGNATURE 0x55AA
#define SIGNATURE_SIZE 2

#define TMP_BUFFER_SIZE 1000

#define BUSY_SLEEP_TIME 200

typedef size_t (*TypeReadCB) (long, uint8_t*, size_t);
typedef size_t (*TypeWriteCB) (long, const uint8_t*, size_t);

class MessageBase {

	uint8_t tmpBuf[TMP_BUFFER_SIZE]{};

	ComponentObject host;

    static CRC::Table<std::uint32_t, 32> crcTable;

public:

    explicit MessageBase(ComponentObject&);

    TypeReadCB getReadCB(long source);
    TypeWriteCB getWriteCB(long source);

    bool transferBinary(long, long, long, uint32_t&);

	bool readBlock(long, uint8_t*, long, uint32_t&);

	bool readSignature(long, uint32_t&);
	bool readHeader(long, uint32_t&);
	bool readBlockHeader(long, MessageBlockHeader&, uint32_t&);
	bool readString(long, char*, long, uint32_t&);
	bool readNumber(long, long&, uint32_t&);
    bool readNumberList(long, std::vector<long> &, long, uint32_t&);
    bool readBinary(long, const char*, long, uint32_t&);
    bool readCRC(long, uint32_t&);
	bool readFromStream(long);

    virtual bool readMessageBlock(long, MessageBlockHeader&, uint32_t&) = 0;


	bool writeBlock(long, const uint8_t *, long, uint32_t&);

	bool writeSignature(long, uint32_t&);
	bool writeHeader(long, uint32_t&);
	bool writeBlockHeader(long, MessageBlockHeader&, uint32_t&);
	bool writeString(long, const char*, uint32_t&);
	bool writeNumber(long, long, uint32_t&);
    bool writeNumberList(long, std::vector<long>&, uint32_t&);
	bool writeBinary(long, const char*, long, uint32_t&);
    bool writeCRC(long, uint32_t&);

	bool writeToStream(long);

    bool writeEndStream(long, uint32_t&);

    virtual bool writeMessageStream(long, uint32_t&) = 0;


    virtual bool deSerializeHeader(const uint8_t*) = 0;
    virtual bool serializeHeader(uint8_t *) = 0;
    virtual long getHeaderSize() = 0;

	ComponentObject& getHost();
};

#endif //DICON_MESSAGEBASE_H
