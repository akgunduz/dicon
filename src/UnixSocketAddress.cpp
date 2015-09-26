//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "UnixSocketAddress.h"

UnixSocketAddress::UnixSocketAddress(long address) {

    set(address);
}

void UnixSocketAddress::set(long address) {

    Address::set(address);

    init();
}

INTERFACES UnixSocketAddress::getInterface() {

    return INTERFACE_UNIXSOCKET;
}

std::string UnixSocketAddress::getString() {

    return std::to_string(getAddress());

}

void UnixSocketAddress::init() {

    bzero((char *) &unix_addr, sizeof(unix_addr));
    unix_addr.sun_family = AF_UNIX;
    sprintf(unix_addr.sun_path, "%s%s%d%s", UNIXSOCKET_PATH, UNIXSOCKET_FILE_PREFIX,
            (uint32_t)address, UNIXSOCKET_FILE_SUFFIX);
    unlink(unix_addr.sun_path);
}

sockaddr_un UnixSocketAddress::getUnixAddress() {

    return unix_addr;

}