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

    bool readID(const TypeComponentUnit&, TypeID&);
    bool readJobName(const TypeComponentUnit&, std::string&);
    bool readFileBinary(const TypeComponentUnit&, const TypeFileItem&);
    bool readProcessID(const TypeComponentUnit&, const TypeProcessItem&);
    bool readProcessCount(const TypeComponentUnit&, uint32_t&);
    bool readProcessInfo(const TypeComponentUnit&, const TypeProcessItem&);
    bool readProcessFileCount(const TypeComponentUnit&, long&);
    bool readProcessFile(const TypeComponentUnit&, const TypeProcessFile&);
    bool readProcessFiles(const TypeComponentUnit&, const TypeProcessItem&);
    bool readProcess(const TypeComponentUnit&, const TypeProcessItem&);
    bool readComponentList(const TypeComponentUnit&, TypeComponentUnitList&);

    bool writeID(const TypeComponentUnit&, const TypeID&);
    bool writeJobName(const TypeComponentUnit&, const std::string&);
    bool writeFileBinary(const TypeComponentUnit&, const TypeFileItem&);
    bool writeProcessID(const TypeComponentUnit&, const TypeProcessItem&);
    bool writeProcessCount(const TypeComponentUnit&, const uint32_t&);
    bool writeProcessInfo(const TypeComponentUnit&, const TypeProcessItem&);
    bool writeProcessFileCount(const TypeComponentUnit&, const TypeProcessItem&);
    bool writeProcessFile(const TypeComponentUnit&, const TypeProcessFile&, bool);
    bool writeProcessFiles(const TypeComponentUnit&, const TypeProcessItem&, bool);
    bool writeProcess(const TypeComponentUnit&, const TypeProcessItem&);
    bool writeComponentList(const TypeComponentUnit&, TypeComponentUnitList&);

    bool writeMessageStream(const TypeComponentUnit& out) override;

public:

	explicit Message(const TypeHostUnit&);
	Message(const TypeHostUnit&, COMPONENT, MSG_TYPE, STREAM_TYPE = STREAM_NONE);
	~Message() = default;

	MessageData& getData();

    bool build(const TypeComponentUnit&) override;
};

typedef std::unique_ptr<Message> TypeMessage;

#endif //DICON_MESSAGE_H
