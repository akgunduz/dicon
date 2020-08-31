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

    MessageData data;

    bool readComponentList(const TypeComponentUnit&, TypeComponentUnitList&, MessageBlockHeader&, uint32_t&);
    bool readNumberList(const TypeComponentUnit&, std::vector<uint64_t> &, size_t, uint32_t&);
    bool readJobName(const TypeComponentUnit&, std::string&, MessageBlockHeader&, uint32_t&);
    bool readProcess(const TypeComponentUnit&, const TypeProcessItem&, MessageBlockHeader&, uint32_t&);
    bool readProcessID(const TypeComponentUnit&, long&, MessageBlockHeader&, uint32_t&);
    bool readFile(const TypeComponentUnit&, const TypeProcessFile&, MessageBlockHeader&, uint32_t&);
    bool readMessageBlock(const TypeComponentUnit& in, MessageBlockHeader&, uint32_t&) override;

    bool writeComponentList(const TypeComponentUnit&, TypeComponentUnitList&, uint32_t&);
    bool writeNumberList(const TypeComponentUnit&, std::vector<uint64_t>&, uint32_t&);
    bool writeJobName(const TypeComponentUnit&, const std::string&, uint32_t&);
    bool writeProcess(const TypeComponentUnit&, const TypeProcessItem&, uint32_t&);
    bool writeProcessID(const TypeComponentUnit&, long, uint32_t&);
    bool writeFile(const TypeComponentUnit&, const TypeProcessFile&, bool, uint32_t&);

    bool writeFileBinary(const TypeComponentUnit&, const TypeFileItem&, uint32_t&);

    bool writeMessageStream(const TypeComponentUnit& out, uint32_t&) override;

public:

	explicit Message(const TypeHostUnit&);
	Message(const TypeHostUnit&, const TypeComponentUnit&, MSG_TYPE);
	~Message() = default;

	MessageData& getData();
};

typedef std::unique_ptr<Message> TypeMessage;
typedef std::map<void*, std::pair<TypeMessage, TypeComponentUnit>> TypeClientMsgList;

#endif //DICON_MESSAGE_H
