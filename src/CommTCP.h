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

    static inline int lastFreeTCPPort = DEFAULT_PORT;
    static inline int lastFreeMulticastPort = DEFAULT_MULTICAST_PORT;

    uv_tcp_t tcpServer{};
    uv_udp_t multicastServer{};

	bool initTCP();
	bool initMulticast();

	static bool onAlloc(uv_buf_t*, size_t, const uint8_t* = nullptr);
	static bool onFree(const uv_buf_t *);
    static bool onTCPShutdown(uv_stream_t*);

    bool onServerConnect();
    bool onClientConnect(const TypeComponentUnit&, TypeMessage&, uv_stream_t*);
    bool onRead(const TypeComponentUnit&, TypeMessage&, const uint8_t*, size_t);

    static void onClose(uv_handle_t*);
    static bool onTCPWrite(const TypeComponentUnit&, const uint8_t*, size_t);
    static bool onMulticastWrite(const TypeComponentUnit&, const uint8_t*, size_t);

	bool runSender(const TypeComponentUnit&, TypeMessage) override;
    bool runMulticastSender(const TypeComponentUnit&, TypeMessage) override;

protected:

    bool initInterface() override;

    bool onShutdown() override;

public :

	CommTCP(const TypeHostUnit&, const TypeDevice&,
            const InterfaceSchedulerCB *);
	~CommTCP() override;

    COMM_INTERFACE getType() override;
	bool isSupportMulticast() override;

    TypeAddressList getAddressList() override;
};

#endif //DICON_COMMTCP_H
