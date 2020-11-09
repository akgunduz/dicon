//
// Created by Haluk AKGUNDUZ on 01.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COLLECTORUNIT_H
#define DICON_COLLECTORUNIT_H

#include "ComponentUnit.h"
#include "CollectorObject.h"

class CollectorUnit : public ComponentUnit, public CollectorObject {

public:
    CollectorUnit(COLLSTATES, ARCH, long, TypeAddress);
    CollectorUnit(ARCH, long, TypeAddress);
    CollectorUnit(const CollectorUnit &);

    ~CollectorUnit() override;
};

typedef std::shared_ptr<CollectorUnit> TypeCollectorUnit;
typedef std::vector<TypeCollectorUnit> TypeCollectorList;

#endif //DICON_COLLECTORUNIT_H
