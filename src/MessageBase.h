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

typedef size_t (*TypeReadCB) (ComponentUnit&, uint8_t*, size_t);
typedef size_t (*TypeWriteCB) (ComponentUnit&, const uint8_t*, size_t);

class MessageBase {

	uint8_t tmpBuf[TMP_BUFFER_SIZE]{};

    const TypeHostUnit host;

    static CRC::Table<std::uint32_t, 32> crcTable;

public:

    explicit MessageBase(const TypeHostUnit&);

    TypeReadCB getReadCB(ComponentUnit& source);
    TypeWriteCB getWriteCB(ComponentUnit& source);

    bool transferBinary(ComponentUnit&, ComponentUnit&, size_t, uint32_t&);

	bool readBlock(ComponentUnit&, uint8_t*, size_t, uint32_t&);

	bool readSignature(ComponentUnit&, uint32_t&);
	bool readHeader(ComponentUnit&, uint32_t&);
	bool readBlockHeader(ComponentUnit&, MessageBlockHeader&, uint32_t&);
	bool readString(ComponentUnit&, char*, size_t, uint32_t&);
	bool readNumber(ComponentUnit&, long&, uint32_t&);
    bool readNumberList(ComponentUnit&, std::vector<long> &, size_t, uint32_t&);
    bool readBinary(ComponentUnit&, const std::filesystem::path&, size_t, uint32_t&);
    bool readCRC(ComponentUnit&, uint32_t&);
	bool readFromStream(ComponentUnit&);

    virtual bool readMessageBlock(ComponentUnit&, MessageBlockHeader&, uint32_t&) = 0;


	bool writeBlock(ComponentUnit&, const uint8_t *, size_t, uint32_t&);

	bool writeSignature(ComponentUnit&, uint32_t&);
	bool writeHeader(ComponentUnit&, uint32_t&);
	bool writeBlockHeader(ComponentUnit&, MessageBlockHeader&, uint32_t&);
	bool writeString(ComponentUnit&, const std::string&, uint32_t&);
	bool writeNumber(ComponentUnit&, long, uint32_t&);
    bool writeNumberList(ComponentUnit&, std::vector<long>&, uint32_t&);
	bool writeBinary(ComponentUnit&, const char*, size_t, uint32_t&);
    bool writeCRC(ComponentUnit&, uint32_t&);

	bool writeToStream(ComponentUnit&);

    bool writeEndStream(ComponentUnit&, uint32_t&);

    virtual bool writeMessageStream(ComponentUnit&, uint32_t&) = 0;

    virtual void deSerializeHeader(const uint8_t*) = 0;
    virtual void serializeHeader(uint8_t *) = 0;
    virtual void grabOwner(ComponentUnit&) = 0;
    virtual long getHeaderSize() = 0;

    const TypeHostUnit& getHost();
};

#endif //DICON_MESSAGEBASE_H
