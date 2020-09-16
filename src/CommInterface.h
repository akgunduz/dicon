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

enum STATUS {
    STATUS_PROGRESS,
    STATUS_DONE,
    STATUS_SHUTDOWN,
};

class CommInterface {

    const TypeHostUnit host;
    const TypeDevice device;

    const CommInterfaceCB *senderCB{};

    bool send(const TypeSchedulerItem& item);

protected :

    Address address{};
    Address multicastAddress{};

	Scheduler *scheduler{};

	std::thread threadProduce;
	std::thread threadConsume;

    uv_loop_t produceLoop{};
    uv_loop_t consumeLoop{};

    static void onAlloc(uv_handle_t*, size_t,uv_buf_t*);
    static void onFree(const uv_buf_t *);
    static void onClose(uv_handle_t*);

    virtual bool initInterface() = 0;
	virtual bool runSender(const TypeComponentUnit&, TypeMessage) = 0;
	virtual bool runMulticastSender(const TypeComponentUnit&, TypeMessage) = 0;

	CommInterface(const TypeHostUnit&, const TypeDevice&, const CommInterfaceCB *);

public :

    virtual ~CommInterface();

	bool push(MSG_DIR, const TypeCommUnit&, TypeMessage);
    const TypeHostUnit& getHost();
    const TypeDevice& getDevice();
    Address& getAddress();
    Address& getMulticastAddress();
    virtual COMM_INTERFACE getType() = 0;
    virtual bool isSupportMulticast() = 0;
    virtual TypeAddressList getAddressList() = 0;

    bool initThread();
    bool waitThread();
    void shutdown();
};

typedef CommInterface* TypeInterface;
typedef std::map<COMPONENT, TypeInterface> TypeInterfaceList;

#endif //DICON_COMMINTERFACE_H
