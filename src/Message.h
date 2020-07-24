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

    bool readComponentList(int, std::vector<ComponentObject>&, MessageBlockHeader&, uint32_t&);
    bool readJobName(int, char*, MessageBlockHeader&, uint32_t&);
    bool readProcess(int, ProcessItem*, MessageBlockHeader&, uint32_t&);
    bool readProcessID(int, long&, MessageBlockHeader&, uint32_t&);
    bool readFile(int, ProcessFile&, MessageBlockHeader&, uint32_t&);
    bool readMessageBlock(int in, MessageBlockHeader&, uint32_t&) override;

    bool writeComponentList(int, std::vector<ComponentObject>&, uint32_t&);
    bool writeJobName(int, const char*, uint32_t&);
    bool writeProcess(int, ProcessItem*, uint32_t&);
    bool writeProcessID(int, long, uint32_t&);
    bool writeFile(int, ProcessFile&, bool, uint32_t&);

    bool writeMessageStream(int out, uint32_t&) override;

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
