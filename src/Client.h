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
	CallBack *mCallback;
	bool mRegistered;
	uint64_t mDistributorAddress;
	uint64_t mCollectorAddress;

	static bool receiveCB(void *, uint64_t, Message *);

public:
	Client(uint32_t, uint32_t, const std::string &rootPath);

	virtual ~Client();
	uint64_t getAddress(HOST);
	INTERFACES getInterfaceType(HOST);

	bool processRule();
	void processExecutor(std::string);
	bool processMD5();

	bool send2DistributorMsg(uint64_t, uint8_t);
	bool send2CollectorMsg(uint64_t, uint8_t);

	bool processCollectorMsg(uint64_t, Message *);
	bool processDistributorMsg(uint64_t, Message *);

	std::string getRootPath();

};


#endif //__Client_H_
