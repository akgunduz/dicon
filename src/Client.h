//
// Created by Haluk AKGUNDUZ on 29/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Client_H_
#define __Client_H_

#include "Common.h"
#include "Connector.h"

class Client {
private:

	Connector *mDistributorConnector;
	Connector *mCollectorConnector;

	enum STATES mState;
	std::list<std::string> mMD5s;

	Rule* mRule;
	std::string mRootPath;
	InterfaceCallback *mCallback;
	bool mRegistered;
	long mDistributorAddress;
	long mCollectorAddress;

	static bool receiveCB(void *, long, Message *);

public:
	Client(uint32_t, uint32_t, const std::string &rootPath);

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

	std::string getRootPath();

};


#endif //__Client_H_
