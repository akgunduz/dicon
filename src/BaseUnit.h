//
// Created by Haluk AKGUNDUZ on 17.09.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_BASEUNIT_H
#define DICON_BASEUNIT_H

#include <cstdint>
#include <vector>
#include "ArchType.h"
#include "ComponentType.h"

typedef uint32_t TypeID;

class BaseUnit {

protected:

    TypeID id{};

    uint16_t arch{ARCH_MAX};

    uint16_t type{COMP_MAX};

public:

    BaseUnit(COMPONENT, ARCH, TypeID);
    explicit BaseUnit(COMPONENT);
    virtual ~BaseUnit() = 0;

    TypeID getID();
    virtual void setID(TypeID);

    ARCH getArch();
    void setArch(ARCH);

    COMPONENT getType();
    void setType(COMPONENT);

    static COMPONENT next(COMPONENT);
    static COMPONENT prev(COMPONENT);
};

typedef std::vector<TypeID> TypeIDList;

#endif //DICON_BASEUNIT_H
