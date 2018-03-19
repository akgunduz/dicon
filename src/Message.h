//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Message_H_
#define __Message_H_

#include "BaseMessage.h"
#include "Unit.h"
#include "Job.h"
#include "Md5.h"

#define STREAM_NONE 0xFFFF

#define STREAM_JOB 0x01

//Send md5 + binary
#define STREAM_BINARY 0x02

//Send md5
#define STREAM_MD5ONLY 0x03

#define BLOCK_FILE_BINARY 0x01
#define BLOCK_FILE_MD5 0x02
#define BLOCK_JOB_INFO 0x03

#define MAX_VARIANT 2

#define MESSAGE_DEFAULT_PRIORITY 3

//To get rid of Alignment problem stay in 64bit mod
struct MessageHeader {

	int type;
	int priority;
	int owner;
	long ownerAddress;
	long time;
	long deviceID;
	long messageID;
	long variant[MAX_VARIANT];
};

class Message : public BaseMessage {

	struct MessageHeader header;

	Unit host;

	int streamFlag;

    char jobDir[50];

public:

    FileList *fileList;

	std::vector<Md5> md5List;

	Message(Unit host);
	Message(Unit owner, int type);

	void setStreamFlag(int);

	int getType();

	Unit getHost();
	void setHost(Unit);
	Unit getOwner();
	void setOwner(Unit);

	long getOwnerAddress();
	void setOwnerAddress(long);
	long getVariant(int id);
	void setVariant(int id, long variant);

	long getTime();
	long getDeviceID();
	long getMessageID();

	int getPriority();
	void setPriority(int);
	void normalizePriority();
	int iteratePriority();

	const char* getJobDir();

	void setJob(int, FileList *fileList);

	bool readMD5(int, Md5*);

	bool readJobInfo(int, char *, struct BlockHeader*);
	bool readFileBinary(int, FileItem *, struct BlockHeader*);
	bool readFileMD5(int, Md5*, struct BlockHeader*);
	bool readMessageBlock(int in, BlockHeader *);

	bool writeMD5(int, Md5*);

	bool writeJobInfo(int, const char *jobDir);
	bool writeFileBinary(int, FileItem *);
	bool writeFileMD5(int, FileItem *);
	bool writeMessageStream(int out);

    virtual bool readFinalize();

    virtual bool writeFinalize();

	bool parseHeader(const uint8_t*);
	bool prepareHeader(uint8_t *);
};

//typedef bool (Message::*fWriteProcess)(int desc, FileItem *content);

#endif //__Message_H_
