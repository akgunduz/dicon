//
// Created by Haluk AKGUNDUZ on 03.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COMMUNIT_H
#define DICON_COMMUNIT_H

#include "BaseUnit.h"
#include "Address.h"

class CommUnit : public BaseUnit {

protected:

    TypeAddress address;

public:

    CommUnit(COMPONENT, ARCH, TypeID, TypeAddress);
    CommUnit(COMPONENT, ARCH, TypeID);
    CommUnit(COMPONENT, TypeAddress);
    explicit CommUnit(COMPONENT = COMP_MAX);
    explicit CommUnit(const BaseUnit*);

    TypeAddress& getAddress();
    void setAddress(const TypeAddress&, bool = false);

    void set(const CommUnit&);

    ~CommUnit() override;

    void setID(TypeID id) override;
    bool deSerialize(const uint8_t*);
    void serialize(uint8_t*);
};

typedef std::shared_ptr<CommUnit> TypeCommUnit;
typedef std::vector<TypeCommUnit> TypeCommUnitList;

#endif //DICON_COMMUNIT_H
