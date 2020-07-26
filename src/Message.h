//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Message_H_
#define __Message_H_

#include "MessageBase.h"
#include "MessageHeader.h"
#include "MessageData.h"

class Message : public MessageBase {

private:
	MessageHeader header{};
    MessageData data{};

    bool readComponentList(long, std::vector<ComponentObject>&, MessageBlockHeader&, uint32_t&);
    bool readJobName(long, char*, MessageBlockHeader&, uint32_t&);
    bool readProcess(long, ProcessItem*, MessageBlockHeader&, uint32_t&);
    bool readProcessID(long, long&, MessageBlockHeader&, uint32_t&);
    bool readFile(long, ProcessFile&, MessageBlockHeader&, uint32_t&);
    bool readMessageBlock(long in, MessageBlockHeader&, uint32_t&) override;

    bool writeComponentList(long, std::vector<ComponentObject>&, uint32_t&);
    bool writeJobName(long, const char*, uint32_t&);
    bool writeProcess(long, ProcessItem*, uint32_t&);
    bool writeProcessID(long, long, uint32_t&);
    bool writeFile(long, ProcessFile&, bool, uint32_t&);

    bool writeMessageStream(long out, uint32_t&) override;

    bool deSerializeHeader(const uint8_t*) override;
    bool serializeHeader(uint8_t *) override;
    long getHeaderSize() override;

public:

	explicit Message(ComponentObject&);
	Message(ComponentObject&, COMPONENT, MSG_TYPE);

	MessageHeader& getHeader();
	MessageData& getData();
};

#endif //__Message_H_
