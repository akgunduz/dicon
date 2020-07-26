//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "Address.h"

long Address::create(INTERFACE interface, long base, int port, int socket, bool multicast) {

    return SET(interface, INTERFACE_POS, INTERFACE_MASK) |
           SET((int)multicast, MULTICAST_POS, MULTICAST_MASK) |
           SET(socket, SOCKET_POS, SOCKET_MASK) |
           SET(port, PORT_POS, PORT_MASK) |
           SET(base, BASE_POS, BASE_MASK) ;
}

INTERFACE Address::getInterface(long& address) {

    return (INTERFACE) GET(address, INTERFACE_POS, INTERFACE_MASK);
}

int Address::getSocket(long& address) {

    return (int) GET(address, SOCKET_POS, SOCKET_MASK);
}

void Address::setSocket(long& address, int socket) {

    address = CLEAR_AND_SET(address, socket, SOCKET_POS, SOCKET_MASK);
}

int Address::getPort(long& address) {

    return (int) GET(address, PORT_POS, PORT_MASK);
}

long Address::getBase(long& address) {

    return (long) GET(address, BASE_POS, BASE_MASK);
}

bool Address::isMulticast(long &address) {

    return (bool) GET(address, MULTICAST_POS, MULTICAST_MASK);
}

void Address::setMulticast(long &address, bool multicast) {

    address = CLEAR_AND_SET(address, (int)multicast, MULTICAST_POS, MULTICAST_MASK);
}
