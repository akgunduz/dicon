//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "Address.h"

Address Address::invalid{};

Address::Address(INTERFACE _interface) {

    flag = SET(flag, _interface, INTERFACE_POS, INTERFACE_MASK);
}

Address::Address(uint32_t _base, uint16_t _port, INTERFACE _interface)
    : self(_base, _port) {

    flag = SET(flag, _interface, INTERFACE_POS, INTERFACE_MASK);
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

void Address::setUI(uint32_t _base, uint16_t _port) {

    if (getInterface() == INTERFACE_NET) {
        ui.base = _base;
        ui.port = _port;
    }
}

void Address::setUI(uint16_t _port) {

    if (getInterface() == INTERFACE_NET) {
        ui.base = self.base;
        ui.port = _port;
    }
}

void Address::set(BaseAddress &ref) {

    self = ref;
}

void Address::setUI(BaseAddress &ref) {

    if (getInterface() == INTERFACE_NET) {
        ui = ref;
    }
}

bool Address::isMulticast() {

    return (bool) GET(flag, MULTICAST_POS, MULTICAST_MASK);
}

INTERFACE Address::getInterface() const {

    return (INTERFACE)GET(flag, INTERFACE_POS, INTERFACE_MASK);
}

void Address::setMulticast(bool _multicast) {

    flag = SET(flag, _multicast, MULTICAST_POS, MULTICAST_MASK);
}

void Address::setInterface(INTERFACE _interface) {

    flag = SET(flag, _interface, INTERFACE_POS, INTERFACE_MASK);
}
