//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_MESSAGE_H
#define DICON_MESSAGE_H

#include "MessageBase.h"
#include "MessageHeader.h"
#include "MessageData.h"

class Message : public MessageBase {

	MessageHeader header{};
    MessageData data{};

    bool readComponentList(ComponentUnit&, TypeComponentUnitList&, MessageBlockHeader&, uint32_t&);
    bool readJobName(ComponentUnit&, char*, MessageBlockHeader&, uint32_t&);
    bool readProcess(ComponentUnit&, const TypeProcessItem&, MessageBlockHeader&, uint32_t&);
    bool readProcessID(ComponentUnit&, long&, MessageBlockHeader&, uint32_t&);
    bool readFile(ComponentUnit&, ProcessFile&, MessageBlockHeader&, uint32_t&);
    bool readMessageBlock(ComponentUnit& in, MessageBlockHeader&, uint32_t&) override;

    bool writeComponentList(ComponentUnit&, TypeComponentUnitList&, uint32_t&);
    bool writeJobName(ComponentUnit&, const char*, uint32_t&);
    bool writeProcess(ComponentUnit&, const TypeProcessItem&, uint32_t&);
    bool writeProcessID(ComponentUnit&, long, uint32_t&);
    bool writeFile(ComponentUnit&, ProcessFile&, bool, uint32_t&);

    bool writeMessageStream(ComponentUnit& out, uint32_t&) override;

    void deSerializeHeader(const uint8_t*) override;
    void serializeHeader(uint8_t *) override;
    void grabOwner(ComponentUnit&) override;
    long getHeaderSize() override;

public:

	explicit Message(HostUnit&);
	Message(HostUnit&, ComponentUnit&, MSG_TYPE);
	~Message();

	MessageHeader& getHeader();
	MessageData& getData();
};

typedef std::unique_ptr<Message> TypeMessage;

#endif //DICON_MESSAGE_H
