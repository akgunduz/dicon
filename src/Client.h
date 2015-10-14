//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Client_H_
#define __Client_H_

#include "Component.h"
#include "Connector.h"

class Client : public Component {
private:

    int distributorIndex;
    int collectorIndex;

	Rule* rule;

	long distributorAddress;

	virtual bool onReceive(long, Message *);

public:
	Client(int, int, const char *rootPath);

	virtual ~Client();
	long getAddress(HOST);
	INTERFACES getInterfaceType(HOST);

	bool processRule();
	void processExecutor(std::string);
	bool processMD5();

	bool send2DistributorMsg(long, uint8_t);
	bool send2CollectorMsg(long, uint8_t);

	bool processCollectorMsg(long, Message *);
	bool processDistributorMsg(long, Message *);
};


#endif //__Client_H_
