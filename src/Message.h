//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Message_H_
#define __Message_H_

#include "BaseMessage.h"
#include "Rule.h"

enum MSG_TYPE {
	MSGTYPE_CLIENT = 100,
	MSGTYPE_RULE,
	MSGTYPE_BINARY,
	MSGTYPE_MD5,
	MSGTYPE_WAKEUP,
	MSGTYPE_READY,
	MSGTYPE_BUSY,
	MSGTYPE_TIME,
	MSGTYPE_TIMEOUT,
	MSGTYPE_ALIVE,
};

#define STREAM_RULE 0x01

//Send md5 + binary
#define STREAM_BINARY 0x02

//Send md5
#define STREAM_MD5ONLY 0x03

#define BLOCK_FILE_BINARY 0x02
#define BLOCK_FILE_MD5 0x03

struct MD5Wrapper {
	uint8_t mMD5[MD5_DIGEST_LENGTH];
};

class Message : public BaseMessage {

public:

	Rule *mRule;
	std::string mRootPath;
	std::vector<MD5Wrapper> mMD5List;

	Message(const std::string&);
	Message(HOST owner, int type, const std::string&);

	void setRule(int, Rule *);
    HOST getOwner();

	bool readMD5(int, uint8_t*);

	bool readFileBinary(int, FileContent *, struct BlockHeader*, const std::string&);
	bool readFileMD5(int, MD5Wrapper *, struct BlockHeader*);
	bool readMessageBlock(int in, BlockHeader *);

	bool writeMD5(int, uint8_t*);

	bool writeFileBinary(int, FileContent  *, const std::string&);
	bool writeFileMD5(int, FileContent *);
	bool writeMessageStream(int out, int streamFlag);

};

#endif //__Message_H_
