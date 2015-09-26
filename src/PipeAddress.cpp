//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#include "PipeAddress.h"

PipeAddress::PipeAddress(long address) {

    set(address);
}

void PipeAddress::set(long address) {

    Address::set(address);
}

INTERFACES PipeAddress::getInterface() {

    return INTERFACE_PIPE;
}

std::string PipeAddress::getString() {

    return std::to_string(getAddress());

}
