//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_MESSAGEBASE_H
#define DICON_MESSAGEBASE_H

#include "crc/CRC.h"
#include "MessageBlockHeader.h"
#include "MessageType.h"
#include "HostUnit.h"
#include "Address.h"

#define SIGNATURE 0x55AA
#define SIGNATURE_SIZE 2

#define TMP_BUFFER_SIZE 1000

#define BUSY_SLEEP_TIME 200

typedef size_t (*TypeReadCB) (const TypeComponentUnit&, uint8_t*, size_t);
typedef size_t (*TypeWriteCB) (const TypeComponentUnit&, const uint8_t*, size_t);

class MessageBase {

	uint8_t tmpBuf[TMP_BUFFER_SIZE]{};

    const TypeHostUnit host;

    static CRC::Table<std::uint32_t, 32> crcTable;

public:

    explicit MessageBase(const TypeHostUnit&);

    TypeReadCB getReadCB(const TypeComponentUnit& source);
    TypeWriteCB getWriteCB(const TypeComponentUnit& source);

    bool transferBinary(const TypeComponentUnit&, const TypeComponentUnit&, size_t, uint32_t&);

	bool readBlock(const TypeComponentUnit&, uint8_t*, size_t, uint32_t&);

	bool readSignature(const TypeComponentUnit&, uint32_t&);
	bool readHeader(const TypeComponentUnit&, uint32_t&);
	bool readBlockHeader(const TypeComponentUnit&, MessageBlockHeader&, uint32_t&);
	bool readString(const TypeComponentUnit&, std::string&, size_t, uint32_t&);
	bool readNumber(const TypeComponentUnit&, long&, uint32_t&);
    bool readNumberList(const TypeComponentUnit&, std::vector<long> &, size_t, uint32_t&);
    bool readBinary(const TypeComponentUnit&, const std::filesystem::path&, size_t, uint32_t&);
    bool readCRC(const TypeComponentUnit&, uint32_t&);
	bool readFromStream(const TypeComponentUnit&);

    virtual bool readMessageBlock(const TypeComponentUnit&, MessageBlockHeader&, uint32_t&) = 0;


	bool writeBlock(const TypeComponentUnit&, const uint8_t *, size_t, uint32_t&);

	bool writeSignature(const TypeComponentUnit&, uint32_t&);
	bool writeHeader(const TypeComponentUnit&, uint32_t&);
	bool writeBlockHeader(const TypeComponentUnit&, MessageBlockHeader&, uint32_t&);
	bool writeString(const TypeComponentUnit&, const std::string&, uint32_t&);
	bool writeNumber(const TypeComponentUnit&, long, uint32_t&);
    bool writeNumberList(const TypeComponentUnit&, std::vector<long>&, uint32_t&);
	bool writeBinary(const TypeComponentUnit&, const char*, size_t, uint32_t&);
    bool writeCRC(const TypeComponentUnit&, uint32_t&);

	bool writeToStream(const TypeComponentUnit&);

    bool writeEndStream(const TypeComponentUnit&, uint32_t&);

    virtual bool writeMessageStream(const TypeComponentUnit&, uint32_t&) = 0;

    virtual void deSerializeHeader(const uint8_t*) = 0;
    virtual void serializeHeader(uint8_t *) = 0;
    virtual void grabOwner(const TypeCommUnit&) = 0;
    virtual long getHeaderSize() = 0;

    const TypeHostUnit& getHost();
};

#endif //DICON_MESSAGEBASE_H
