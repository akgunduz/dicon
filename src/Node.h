//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef NODE_H
#define NODE_H

#include "Component.h"

class Node : public Component {
private:

	char executor[PATH_MAX];
	long distributorAddress;

	const char* getExecutor();
	void setExecutor(char*);
	long getDistributorAddress();
	void setDistributorAddress(long);

    virtual bool processDistributorMsg(long, Message *);
    virtual bool processCollectorMsg(long, Message *);
    virtual bool processNodeMsg(long, Message *);

    bool send2DistributorMsg(long, MSG_TYPE, ...);
    bool send2CollectorMsg(long, MSG_TYPE, ...);

    void parseCommand(char *cmd, char **argv);
    bool processCommand(const char*);

	TypeFileList checkFileExistence(TypeFileList);

public:
	Node(const char *rootPath);

	virtual ~Node();
};


#endif //NODE_H
