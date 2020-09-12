//
// Created by Haluk AKGUNDUZ on 3.08.2020.
//

#ifndef DICON_COMMUNIT_H
#define DICON_COMMUNIT_H

#include "Address.h"
#include "ComponentType.h"
#include "ArchType.h"

typedef uint32_t TypeID;

class CommUnit {

protected:

    TypeID id{};

    uint16_t arch{ARCH_MAX};

    uint16_t type{COMP_MAX};

    Address address{};

    friend class MessageHeader;

public:

    CommUnit() = default;
    CommUnit(COMPONENT, ARCH, TypeID, Address);
    CommUnit(COMPONENT, ARCH, TypeID);
    CommUnit(COMPONENT, Address);
    explicit CommUnit(COMPONENT);

    TypeID getID();

    virtual void setID(TypeID);

    ARCH getArch();
    void setArch(ARCH);

    COMPONENT getType();
    void setType(COMPONENT);

    Address& getAddress();
    void setAddress(Address, bool = false);

    void set(const CommUnit&);

    static COMPONENT next(COMPONENT);
    static COMPONENT prev(COMPONENT);
};

typedef std::shared_ptr<CommUnit> TypeCommUnit;
typedef std::vector<TypeCommUnit> TypeCommUnitList;

#endif //DICON_COMMUNIT_H
