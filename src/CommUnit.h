//
// Created by Haluk AKGUNDUZ on 3.08.2020.
//

#ifndef DICON_COMMUNIT_H
#define DICON_COMMUNIT_H

#include "Address.h"
#include "ComponentType.h"
#include "ArchType.h"

class CommUnit {

protected:

    uint32_t id{};

    uint16_t arch{ARCH_MAX};

    uint16_t type{COMP_MAX};

    Address address{};

    friend class MessageHeader;

public:

    CommUnit() = default;
    explicit CommUnit(COMPONENT);
    CommUnit(COMPONENT, Address);
    CommUnit(COMPONENT, ARCH, long, Address);
    CommUnit(const CommUnit&);

    long getID() const;
    void setID(long);

    ARCH getArch() const;
    void setArch(ARCH);

    COMPONENT getType() const;
    void setType(COMPONENT);

    Address& getAddress();
    void setAddress(Address, bool = false);

    void grab(CommUnit&);

    static COMPONENT next(COMPONENT);
    static COMPONENT prev(COMPONENT);
};

#endif //DICON_COMMUNIT_H
