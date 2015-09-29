//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "UnixSocketAddress.h"

sockaddr_un UnixSocketAddress::getUnixAddress(long address) {

    sockaddr_un unix_addr;
    bzero((char *) &unix_addr, sizeof(unix_addr));
    unix_addr.sun_family = AF_UNIX;
    sprintf(unix_addr.sun_path, "%s%s%d%s", UNIXSOCKET_PATH, UNIXSOCKET_FILE_PREFIX,
            (uint32_t)address, UNIXSOCKET_FILE_SUFFIX);
    unlink(unix_addr.sun_path);

}