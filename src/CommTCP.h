//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COMMTCP_H
#define DICON_COMMTCP_H

#include "Common.h"
#include "Scheduler.h"
#include "CommInterface.h"
#include "Message.h"
#include "Component.h"
#include "CommInterfaceType.h"

#define LOOPBACK_RANGE 256
#define MULTICAST_ADDRESS 0xE9010101
#define DEFAULT_PORT 61001
#define DEFAULT_MULTICAST_PORT 62001
#define TRY_COUNT 10

class CommTCP : public CommInterface {

    uv_tcp_t tcpServer{};
    uv_udp_t multicastServer{};

    static inline int lastFreeTCPPort = DEFAULT_PORT;
    static inline int lastFreeMulticastPort = DEFAULT_MULTICAST_PORT;

	int netSocket{};
	int multicastSocket{};

	bool initTCP();
	bool initMulticast();

	bool onAlloc(size_t suggested_size, uv_buf_t *buf);
	bool onFree(const uv_buf_t *);
    bool onConnection();
    bool onShutdown(uv_stream_t*);
    bool onRead(TypeComponentUnit&, TypeMessage&, const uint8_t*, size_t);
    bool onWrite(const TypeComponentUnit& target, const uint8_t*, size_t);

	bool runSender(const TypeComponentUnit&, TypeMessage) override;
    bool runMulticastSender(const TypeComponentUnit&, TypeMessage) override;

public :

	CommTCP(const TypeHostUnit&, const TypeDevice&,
            const InterfaceSchedulerCB *);
	~CommTCP() override;

    COMM_INTERFACE getType() override;
	bool isSupportMulticast() override;

    TypeAddressList getAddressList() override;
};

struct CommData {

    TypeMessage msg;
    TypeComponentUnit component;
    CommTCP *interface;
};


#endif //DICON_COMMTCP_H
