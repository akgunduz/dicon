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
	Address* mDistributorAddress;
	Address* mCollectorAddress;

	static bool receiveCB(void *, Address *, Message *);

public:
	Client(uint32_t, uint32_t, const std::string &rootPath);

	virtual ~Client();
	Address* getAddress(HOST);
	INTERFACES getInterfaceType(HOST);

	bool processRule();
	void processExecutor(std::string);
	bool processMD5();

	bool send2DistributorMsg(Address*, uint8_t);
	bool send2CollectorMsg(Address*, uint8_t);

	bool processCollectorMsg(Address*, Message *);
	bool processDistributorMsg(Address*, Message *);

	std::string getRootPath();

};


#endif //__Client_H_
