//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#ifndef DICON_COLLECTORUNIT_H
#define DICON_COLLECTORUNIT_H

#include "Address.h"
#include "CollectorState.h"
#include "ComponentUnit.h"
#include "CollectorObject.h"

class CollectorUnit : public ComponentUnit, public CollectorObject {

public:
    CollectorUnit(COLLSTATES, ARCH, long, Address);
    explicit CollectorUnit(ARCH, long, Address);
    CollectorUnit();
    CollectorUnit(const CollectorUnit &);
    CollectorUnit(ComponentUnit&);

    ~CollectorUnit();
};

typedef std::vector<CollectorUnit> TypeCollectorList;

#endif //DICON_COLLECTORUNIT_H
