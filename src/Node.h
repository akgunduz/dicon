//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef NODE_H
#define NODE_H

#include "Component.h"

class Node : public Component {
private:

	//Job* job;

	char executor[PATH_MAX];
	TypeMD5List md5List;

	long distributorAddress;
	bool setDistributorAddress(long);

    virtual bool processDistributorMsg(long, Message *);
    virtual bool processCollectorMsg(long, Message *);
    virtual bool processNodeMsg(long, Message *);

    bool send2DistributorMsg(long, MSG_TYPE, ...);
    bool send2CollectorMsg(long, MSG_TYPE, ...);

    void parseCommand(char *cmd, char **argv);
    bool processRule();
    bool processMD5();
    bool processParallel(Job*);
    bool processSequential(Job*);

    void setExecutor(char*);
    TypeMD5List checkFileExistence(TypeMD5List *);

public:
	Node(const char *rootPath);

	virtual ~Node();
};


#endif //NODE_H
