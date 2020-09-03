//
// Created by Haluk Akgunduz on 24/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//
#ifndef DICON_COMMINTERFACE_H
#define DICON_COMMINTERFACE_H

#include "Scheduler.h"
#include "Message.h"
#include "Address.h"
#include "Device.h"
#include "MessageItem.h"

#define MAX_SIMUL_CLIENTS 100
#define DEFAULT_WEB_PORT 8081

enum NOTIFIER_TYPE {
	NOTIFIER_READ,
	NOTIFIER_WRITE
};

enum DATA_STATE {

    DATASTATE_INIT,
    DATASTATE_PROCESS
};

struct ReceiveData {

    TypeMessage msg;
    TypeComponentUnit unit;
    DATA_STATE state;
};

struct SendData {

    TypeMessage msg;
    TypeComponentUnit unit;
    DATA_STATE state;
};

class CommInterface {

    const TypeHostUnit host;
    const TypeDevice device;

    Address address{};
    Address multicastAddress{};

    const InterfaceSchedulerCB *senderCB{};

protected :

    uv_loop_t loop{};
	Scheduler *scheduler;
	std::thread threadRcv;
	int notifierPipe[2]{};

    ReceiveData receiveData[2];

    virtual bool onRead(ReceiveData&, ssize_t, const uv_buf_t*) = 0;
	virtual bool runSender(const TypeComponentUnit&, TypeMessage) = 0;
	virtual bool runMulticastSender(const TypeComponentUnit&, TypeMessage) = 0;

	bool initThread();
	void end();
	CommInterface(const TypeHostUnit&, const TypeDevice&, const InterfaceSchedulerCB *);

public :

    virtual ~CommInterface();

	bool push(MSG_DIR, const TypeCommUnit&, TypeMessage);
    const TypeHostUnit& getHost();
    const TypeDevice& getDevice();
    Address& getAddress();
    Address& getMulticastAddress();

    void setAddress(Address&);
    void setMulticastAddress(Address&);

    virtual COMM_INTERFACE getType() = 0;
    virtual bool isSupportMulticast() = 0;
    virtual TypeAddressList getAddressList() = 0;
};

typedef std::shared_ptr<CommInterface> TypeInterface;
typedef std::map<COMPONENT, TypeInterface> TypeInterfaceList;

#endif //DICON_COMMINTERFACE_H
