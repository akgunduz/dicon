//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "Address.h"

Address Address::invalid{};

Address::Address(COMM_INTERFACE _interface) {

    flag = SET(flag, _interface, INTERFACE_POS, INTERFACE_MASK);
}

Address::Address(uint32_t _base, uint16_t _port, bool _multicast)
    : self(_base, _port) {

    flag = SET(flag, _multicast, MULTICAST_POS, MULTICAST_MASK);
}

Address::Address(BaseAddress _self)
    : self(_self) {
}

Address::Address(BaseAddress _self, BaseAddress _ui)
    : self(_self), ui(_ui) {
}

BaseAddress& Address::get() {
    return self;
}

BaseAddress& Address::getUI() {
    return ui;
}

void Address::set(uint32_t _base, uint16_t _port) {

    self.base = _base;
    self.port = _port;
}

void Address::setPort(uint16_t _port) {

    self.port = _port;
}


void Address::setUI(uint32_t _base, uint16_t _port) {

    if (getInterface() == COMMINTERFACE_TCPIP) {
        ui.base = _base;
        ui.port = _port;
    }
}

void Address::setUI(uint16_t _port) {

    if (getInterface() == COMMINTERFACE_TCPIP) {
        ui.base = self.base;
        ui.port = _port;
    }
}

void Address::set(BaseAddress &ref) {

    self = ref;
}

void Address::setUI(BaseAddress &ref) {

    if (getInterface() == COMMINTERFACE_TCPIP) {
        ui = ref;
    }
}

bool Address::isMulticast() {

    return (bool) GET(flag, MULTICAST_POS, MULTICAST_MASK);
}

COMM_INTERFACE Address::getInterface() const {

    return (COMM_INTERFACE)GET(flag, INTERFACE_POS, INTERFACE_MASK);
}

void Address::setMulticast(bool _multicast) {

    flag = SET(flag, _multicast, MULTICAST_POS, MULTICAST_MASK);
}

void Address::setInterface(COMM_INTERFACE _interface) {

    flag = SET(flag, _interface, INTERFACE_POS, INTERFACE_MASK);
}
