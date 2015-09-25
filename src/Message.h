//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Message_H_
#define __Message_H_

#include "Common.h"
#include "Rule.h"
/*
enum MSG_OWNER {
	MSGOWN_DISTRIBUTOR,
	MSGOWN_COLLECTOR,
	MSGOWN_CLIENT,
};
*/
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

enum PRIORITIES {
	PRIORITY_HEAD,
	PRIORITY_1,
	PRIORITY_2,
	PRIORITY_3,
	PRIORITY_4,
	PRIORITY_5,
	PRIORITY_6,
	PRIORITY_7,
	PRIORITY_8,
	PRIORITY_9,
	PRIORITY_MAX = PRIORITY_9
};

#define PRIORITY_COEFFICIENT 10

#define DEFAULT_PRIORITY PRIORITY_3

#define STREAM_NONE 0x00


#define STREAM_RULE 0x01

//Send file md5 + path + params + executors
#define STREAM_INFO 0x04

//Send md5 + binary
#define STREAM_BINARY 0x02

//Send md5
#define STREAM_MD5ONLY 0x03

#define SIGNATURE 0xAA55

#define BLOCK_FILE_INFO 0x01
#define BLOCK_FILE_BINARY 0x02
#define BLOCK_FILE_MD5 0x03
#define BLOCK_PARAM 0x04
#define BLOCK_EXECUTOR 0x05
#define BLOCK_END_STREAM 0x06

enum VARIANT_TYPE {
	VARIANT_SMALL,
	VARIANT_BIG
};

struct MD5Wrapper {
	uint8_t mMD5[MD5_DIGEST_LENGTH];
};

//To get rid of Alignment problem stay in 64bit mod
struct MessageHeader {
	uint8_t mOwner;
	uint8_t mTarget;
	uint8_t mPriority;
	uint8_t mType;
	uint32_t mPadding;
	uint64_t mOwnerAddress;
	uint64_t mVariant;
};

struct BlockHeader {
	uint16_t mBlockType;
	uint16_t mPadding;
	uint32_t mSize1;
	uint32_t mSize2;
	uint32_t mSize3;
};

class Message {
	uint8_t mTmpBuf[1000];
	struct MessageHeader mHeader;
	uint8_t mStreamFlag;
	ssize_t mReceivedBinaryCount;
public:

	Rule *mRule;
	std::string mRootPath;
	std::vector<MD5Wrapper> mMD5List;

	Message(const std::string&);

	HOST getOwner();
	void setOwner(HOST);

	HOST getTarget();
	void setTarget(HOST);

	uint8_t getType();
	void setType(uint8_t);

	PRIORITIES getPriority();
	void setPriority(PRIORITIES);
	void normalizePriority();
	PRIORITIES iteratePriority();

	void setStreamFlag(uint8_t);

	uint64_t getOwnerAddress();
	void setOwnerAddress(uint64_t);

	uint64_t getVariant();
	void setVariant(uint64_t);

	void setRule(uint8_t, Rule *);

	ssize_t getReceivedBinaryCount();

	void display();

	std::string getPath(uint8_t*);

	bool readBlock(int, uint8_t *, uint32_t);
	bool readBinary(int, const std::string&, uint8_t *, uint32_t);
	bool transferBinary(int, int, uint8_t *, uint32_t);
	uint32_t getBinarySize(const std::string&);

	bool readSignature(int);
	bool readHeader(int, struct MessageHeader *);
	bool readBlockHeader(int, struct BlockHeader *);
	bool readMD5(int, uint8_t*);
	bool readString(int, std::string&, uint32_t);
	bool readCharArray(int, char *, uint32_t);
	bool readNumber(int, uint64_t*, uint32_t);

	bool readFileInfo(int, FileContent *, uint32_t);
	bool readFileBinary(int, FileContent *, struct BlockHeader*, const std::string&);
	bool readFileMD5(int, MD5Wrapper *, struct BlockHeader*);
	bool readParameter(int, ParamContent *, uint32_t);
	bool readExecutor(int, ExecutorContent *, uint32_t);

	bool readFromStream(int);

	bool writeBlock(int, const uint8_t *, uint32_t);

	bool writeSignature(int);
	bool writeHeader(int, struct MessageHeader *);
	bool writeBlockHeader(int, struct BlockHeader *);
	bool writeMD5(int, uint8_t*);
	bool writeString(int, const std::string&);
	bool writeCharArray(int, const char *);
	bool writeNumber(int, uint64_t);
	bool writeBinary(int, const std::string&, uint8_t *, uint32_t);

	bool writeFileInfo(int, FileContent *);
	bool writeFileBinary(int, FileContent  *, const std::string&);
	bool writeFileMD5(int, FileContent *);

	bool writeParameter(int, ParamContent *);
	bool writeExecutor(int, ExecutorContent *);

	bool writeEndStream(int);

	bool writeToStream(int);

	int readcheck(int, void*, size_t);

};

#endif //__Message_H_
