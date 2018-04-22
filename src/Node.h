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

    bool processDistributorWakeupMsg(long, Message *);
    bool processCollectorJobMsg(long, Message *);
    bool processCollectorBinaryMsg(long, Message *);

    bool send2DistributorReadyMsg(long);
    bool send2DistributorAliveMsg(long);
    bool send2DistributorBusyMsg(long);
    bool send2CollectorInfoMsg(long, TypeFileList*);

    void parseCommand(char *cmd, char **argv);
    bool processCommand(const char*);

	TypeFileList checkFileExistence(TypeFileList);

public:
	Node(const char *rootPath);

	virtual ~Node();
};


#endif //NODE_H
