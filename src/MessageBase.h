//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_MESSAGEBASE_H
#define DICON_MESSAGEBASE_H

#include "crc/CRC.h"
#include "uv.h"
#include "MessageBlock.h"
#include "MessageType.h"
#include "HostUnit.h"
#include "ComponentUnit.h"
#include "Address.h"
#include "MessageHeader.h"
#include "FileItem.h"

#define TMP_BUFFER_SIZE 1000

#define BUSY_SLEEP_TIME 200

class MessageBase;
typedef size_t (*TypeReadCB) (const TypeComponentUnit&, uint8_t*, size_t);
typedef bool (*TypeWriteCB) (const TypeComponentUnit&, const uint8_t*, size_t);
typedef bool (MessageBase::*TypeMsgReadParser)(const TypeComponentUnit&, const uint8_t*, size_t);

enum MSG_STATE {

    MSGSTATE_INIT,
    MSGSTATE_DATA,
    MSGSTATE_BINARY,
    MSGSTATE_MAX
};

class MessageBase {

    uint64_t iter = 1;

    MSG_STATE state{MSGSTATE_INIT};

    uint8_t tmpBuf[TMP_BUFFER_SIZE]{};

    size_t tmpBufPos = 0;
    size_t binBufPos = 0;

    uint32_t crc{};

    MessageBlock block{MSGHEADER_MAX, 0};

    MessageHeader header;

    const TypeHostUnit host;

    std::map<MSG_HEADER, TypeMsgReadParser> readParser;

    TypeWriteCB writeData;

protected:

    std::deque<uint64_t> numbers;
    std::deque<std::string> strings;

public:

    explicit MessageBase(const TypeHostUnit&);
    MessageBase(const TypeHostUnit&, MSG_TYPE, STREAM_TYPE);
    ~MessageBase();

    bool onRead(const TypeComponentUnit&, const uint8_t*, size_t);

	bool readBlock(const TypeComponentUnit&, const uint8_t*, size_t);
	bool readHeader(const TypeComponentUnit&, const uint8_t*, size_t);
	bool readString(const TypeComponentUnit&, const uint8_t*, size_t);
	bool readNumber(const TypeComponentUnit&, const uint8_t*, size_t);
    bool readBinary(const TypeComponentUnit&, const uint8_t*, size_t);
    bool readEndStream(const TypeComponentUnit&, const uint8_t*, size_t);

    virtual bool build(const TypeComponentUnit&) = 0;

	bool onWrite(const TypeComponentUnit&, MSG_HEADER, const uint8_t*, size_t);

	bool writeHeader(const TypeComponentUnit&);
	bool writeString(const TypeComponentUnit&, const std::string&);
	bool writeNumber(const TypeComponentUnit&, uint64_t);
	bool writeBinary(const TypeComponentUnit&, const TypeFileItem&);

    bool writeEndStream(const TypeComponentUnit&);

	bool writeToStream(const TypeComponentUnit&, TypeWriteCB);

    virtual bool writeMessageStream(const TypeComponentUnit&) = 0;

    bool deSerializeHeader(const uint8_t*);
    void serializeHeader(uint8_t *);
    void grabOwner(const TypeCommUnit&);

    const TypeHostUnit& getHost();
    MessageHeader& getHeader();
};

#endif //DICON_MESSAGEBASE_H
