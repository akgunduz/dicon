//
// Created by Haluk AKGUNDUZ on 15/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COMMPIPE_H
#define DICON_COMMPIPE_H

#include <atomic>
#include "CommInterface.h"

#define DEFAULT_PIPE_PORT 63001
#define UNIXSOCKETADDRESS_MASK 0xFFFFFFFFFFFFFF

class CommPipe : public CommInterface {

    static inline std::atomic_uint16_t lastFreePipePort = DEFAULT_PIPE_PORT;

    uv_pipe_t *pipeServer{};

	bool initPipe();

	static bool onReceive(uv_handle_t*, ssize_t, const uv_buf_t *);
    bool onServerConnect();

    static bool onSendCB(const TypeComponentUnit&, const uint8_t*, size_t);
    bool onSend(const TypeComponentUnit&, TypeMessage) override;

protected:

    bool initInterface() override;

public :

    CommPipe(TypeHostUnit, const TypeDevice&,
            const CommInterfaceCB *);
	~CommPipe() override;

    COMM_INTERFACE getType() override;
	bool isSupportMulticast() override;

    TypeAddressList getAddressList() override;
};

#endif //DICON_COMMPIPE_H
