//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Message_H_
#define __Message_H_

#include "BaseMessage.h"
#include "Job.h"
#include "MessageHeader.h"

#define STREAM_NONE 0xFFFF

#define STREAM_JOB 0x01

//Send md5 + binary
#define STREAM_BINARY 0x02

//Send md5
#define STREAM_MD5ONLY 0x03

#define BLOCK_FILE_BINARY 0x01
#define BLOCK_FILE_MD5 0x02
#define BLOCK_JOB_INFO 0x03

class Message : public BaseMessage {

	MessageHeader header;

	Unit host;

	int streamFlag;

    char jobDir[50];

public:

    FileList *fileList;

	std::vector<Md5> md5List;

	Message(Unit host);
	Message(Unit owner, MSG_TYPE type);

	void setStreamFlag(int);

	MessageHeader *getHeader();

	Unit getHost();
	void setHost(Unit);

	const char* getJobDir();

	void setJob(int, FileList *fileList);

	bool readMD5(int, Md5*);

	bool readJobInfo(int, char *, struct Block*);
	bool readFileBinary(int, FileItem *, struct Block*);
	bool readFileMD5(int, Md5*, struct Block*);
	bool readMessageBlock(int in, Block*);

	bool writeMD5(int, Md5*);

	bool writeJobInfo(int, const char *jobDir);
	bool writeFileBinary(int, FileItem *);
	bool writeFileMD5(int, FileItem *);
	bool writeMessageStream(int out);

    virtual bool readFinalize();

    virtual bool writeFinalize();

	bool setHeader(const uint8_t*);
	bool extractHeader(uint8_t *);
};

#endif //__Message_H_
