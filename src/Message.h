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

    bool readJobName(const TypeComponentUnit&, std::string&);
    bool readFileBinary(const TypeComponentUnit&, const TypeFileItem&);
    bool readProcessID(const TypeComponentUnit&, const TypeProcessItem&);
    bool readProcessInfo(const TypeComponentUnit&, const TypeProcessItem&);
    bool readProcessFileCount(const TypeComponentUnit&, long&);
    bool readProcessFile(const TypeComponentUnit&, const TypeProcessFile&);
    bool readProcessFiles(const TypeComponentUnit&, const TypeProcessItem&);
    bool readProcess(const TypeComponentUnit&, const TypeProcessItem&);
    bool readComponentList(const TypeComponentUnit&, TypeComponentUnitList&);

    bool writeJobName(const TypeComponentUnit&, const std::string&, uint32_t&);
    bool writeFileBinary(const TypeComponentUnit&, const TypeFileItem&, uint32_t&);
    bool writeProcessID(const TypeComponentUnit&, const TypeProcessItem&, uint32_t&);
    bool writeProcessInfo(const TypeComponentUnit&, const TypeProcessItem&, uint32_t&);
    bool writeProcessFileCount(const TypeComponentUnit&, const TypeProcessItem&, uint32_t& crc);
    bool writeProcessFile(const TypeComponentUnit&, const TypeProcessFile&, bool, uint32_t&);
    bool writeProcessFiles(const TypeComponentUnit&, const TypeProcessItem&, bool, uint32_t&);
    bool writeProcess(const TypeComponentUnit&, const TypeProcessItem&, uint32_t&);
    bool writeComponentList(const TypeComponentUnit&, TypeComponentUnitList&, uint32_t&);

    bool writeMessageStream(const TypeComponentUnit& out, uint32_t&) override;

public:

	explicit Message(const TypeHostUnit&);
	Message(const TypeHostUnit&, const TypeComponentUnit&, MSG_TYPE, STREAM_TYPE = STREAM_NONE);
	~Message() = default;

	MessageData& getData();

    bool build(const TypeComponentUnit&) override;
};

typedef std::unique_ptr<Message> TypeMessage;
typedef std::map<void*, std::pair<TypeMessage, TypeComponentUnit>> TypeClientMsgList;

#endif //DICON_MESSAGE_H
