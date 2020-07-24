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

class MessageBase {

	uint8_t tmpBuf[TMP_BUFFER_SIZE]{};

	ComponentObject host;

	sockaddr_in datagramAddress{};

    static CRC::Table<std::uint32_t, 32> crcTable;

public:

    explicit MessageBase(ComponentObject&);

    void setDatagramAddress(sockaddr_in);

    bool transferBinary(int, int, long, uint32_t&);

	bool readBlock(int, uint8_t*, long, uint32_t&);

	bool readSignature(int, uint32_t&);
	bool readHeader(int, uint32_t&);
	bool readBlockHeader(int, MessageBlockHeader&, uint32_t&);
	bool readString(int, char*, long, uint32_t&);
	bool readNumber(int, long&, uint32_t&);
    bool readNumberList(int, std::vector<long> &, long, uint32_t&);
    bool readBinary(int, const char*, long, uint32_t&);
    bool readCRC(int, uint32_t&);
	bool readFromStream(int);

    virtual bool readMessageBlock(int, MessageBlockHeader&, uint32_t&) = 0;


	bool writeBlock(int, const uint8_t *, long, uint32_t&);

	bool writeSignature(int, uint32_t&);
	bool writeHeader(int, uint32_t&);
	bool writeBlockHeader(int, MessageBlockHeader&, uint32_t&);
	bool writeString(int, const char*, uint32_t&);
	bool writeNumber(int, long, uint32_t&);
    bool writeNumberList(int, std::vector<long>&, uint32_t&);
	bool writeBinary(int, const char*, long, uint32_t&);
    bool writeCRC(int, uint32_t&);

	bool writeToStream(int);

    bool writeEndStream(int, uint32_t&);

    virtual bool writeMessageStream(int, uint32_t&) = 0;


    virtual bool deSerializeHeader(const uint8_t*) = 0;
    virtual bool serializeHeader(uint8_t *) = 0;
    virtual long getHeaderSize() = 0;

	ComponentObject& getHost();
};

#endif //DICON_MESSAGEBASE_H
