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
    explicit CommUnit(COMPONENT);
    CommUnit(COMPONENT, Address);
    CommUnit(COMPONENT, ARCH, TypeID, Address);
    CommUnit(const CommUnit&);

    TypeID getID() const;
    void setID(TypeID);

    ARCH getArch() const;
    void setArch(ARCH);

    COMPONENT getType() const;
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
