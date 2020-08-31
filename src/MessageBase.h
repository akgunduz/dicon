//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_MESSAGEBASE_H
#define DICON_MESSAGEBASE_H

#include "crc/CRC.h"
#include "uv.h"
#include "MessageBlockHeader.h"
#include "MessageType.h"
#include "HostUnit.h"
#include "Address.h"
#include "MessageHeader.h"

#define TMP_BUFFER_SIZE 1000

#define BUSY_SLEEP_TIME 200

class MessageBase;
typedef size_t (*TypeReadCB) (const TypeComponentUnit&, uint8_t*, size_t);
typedef size_t (*TypeWriteCB) (const TypeComponentUnit&, const uint8_t*, size_t);
typedef bool (MessageBase::*TypeMsgReadParser)(const TypeComponentUnit&, const uint8_t*, size_t, uint32_t&);

enum MSG_STATE {

    MSGSTATE_INIT,
    MSGSTATE_DATA,
    MSGSTATE_BINARY,
    MSGSTATE_MAX
};

enum MSG_HEADER {

    MSGHEADER_HEADER = 0xA1,
    MSGHEADER_NUMBER,
    MSGHEADER_STRING,
    MSGHEADER_BINARY,
    MSGHEADER_END,
    MSGHEADER_MAX
};

struct MessageBlock {

    uint16_t sign{SIGNATURE};
    uint16_t type{MSGHEADER_HEADER};
    uint32_t size{};
    uint32_t binSize{};
};

class MessageBase {

    MSG_STATE state{MSGSTATE_INIT};

    uint8_t tmpBuf[TMP_BUFFER_SIZE]{};

    size_t tmpBufPos = 0;
    size_t binBufPos = 0;

    uint32_t crc;

    MessageBlock block;

    MessageHeader header;

    const TypeHostUnit host;

    std::map<MSG_HEADER, TypeMsgReadParser> readParser;

    static inline CRC::Table<std::uint32_t, 32> crcTable{CRC::CRC_32()};

protected:

    std::deque<uint64_t> numbers;
    std::deque<std::string> strings;

public:

    explicit MessageBase(const TypeHostUnit&);
    MessageBase(const TypeHostUnit&, const TypeComponentUnit&, MSG_TYPE, STREAM_TYPE);

    bool onRead(const TypeComponentUnit&, ssize_t, const uv_buf_t *);

    TypeReadCB getReadCB(const TypeComponentUnit& source);
    TypeWriteCB getWriteCB(const TypeComponentUnit& source);

    bool transferBinary(const TypeComponentUnit&, const TypeComponentUnit&, size_t, uint32_t&);

	bool readBlockDeprecated(const TypeComponentUnit&, uint8_t*, size_t, uint32_t&);

	bool readBlock(const TypeComponentUnit&, const uint8_t*, size_t, uint32_t&);
	bool readHeader(const TypeComponentUnit&, const uint8_t*, size_t, uint32_t&);
	bool readString(const TypeComponentUnit&, const uint8_t*, size_t, uint32_t&);
	bool readNumber(const TypeComponentUnit&, const uint8_t*, size_t, uint32_t&);
    bool readBinary(const TypeComponentUnit&, const uint8_t*, size_t, uint32_t&);
    bool readEndStream(const TypeComponentUnit&, const uint8_t*, size_t, uint32_t&);

	bool readFromStream(const TypeComponentUnit&);

    virtual bool readMessageBlock(const TypeComponentUnit&, MessageBlockHeader&, uint32_t&) = 0;
    virtual bool build(const TypeComponentUnit&) = 0;


	bool writeBlock(const TypeComponentUnit&, const uint8_t *, size_t, uint32_t&);

	bool writeHeader(const TypeComponentUnit&, uint32_t&);
	bool writeBlockBase(const TypeComponentUnit&, const MessageBlock&, uint32_t&);
	bool writeBlockHeader(const TypeComponentUnit&, MessageBlockHeader&, uint32_t&);
	bool writeString(const TypeComponentUnit&, const std::string&, uint32_t&);
	bool writeNumber(const TypeComponentUnit&, uint64_t, uint32_t&);
	bool writeBinary(const TypeComponentUnit&, const std::filesystem::path&, size_t, uint32_t&);

    bool writeEndStream(const TypeComponentUnit&, uint32_t&);

	bool writeToStream(const TypeComponentUnit&);

    virtual bool writeMessageStream(const TypeComponentUnit&, uint32_t&) = 0;
    
    bool deSerializeHeader(const uint8_t*);
    void serializeHeader(uint8_t *);
    void grabOwner(const TypeCommUnit&);

    const TypeHostUnit& getHost();
    MessageHeader& getHeader();
};

#endif //DICON_MESSAGEBASE_H
