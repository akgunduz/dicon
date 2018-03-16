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

#define STREAM_JOB 0x01

//Send md5 + binary
#define STREAM_BINARY 0x02

//Send md5
#define STREAM_MD5ONLY 0x03

#define BLOCK_FILE_BINARY 0x01
#define BLOCK_FILE_MD5 0x02
#define BLOCK_JOB_INFO 0x03

class Message : public BaseMessage {

    char jobDir[50];
    void setRootPath(const char *);

public:

    //char rootPath[PATH_MAX];

	//Job *job;
    FileList *fileList;

	std::vector<Md5> md5List;

//	Message(Unit host, const char*);
//	Message(Unit owner, int type, const char*);
	Message(Unit host);
	Message(Unit owner, int type);

//    const char* getRootPath();
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
	bool writeMessageStream(int out, int streamFlag);

    virtual bool readFinalize();

    virtual bool writeFinalize();
};

//typedef bool (Message::*fWriteProcess)(int desc, FileItem *content);

#endif //__Message_H_
