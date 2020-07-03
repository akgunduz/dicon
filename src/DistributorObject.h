//
// Created by Haluk AKGUNDUZ on 03.07.2020.
//

#ifndef DICON_DISTRIBUTOROBJECT_H
#define DICON_DISTRIBUTOROBJECT_H

#include "ComponentObject.h"

class DistributorObject : public ComponentObject {

private:


public:
    DistributorObject();
    DistributorObject(DistributorObject &);
    DistributorObject(ComponentObject &);
    DistributorObject(const char*);
    DistributorObject(int, long);
    ~DistributorObject();
};


#endif //DICON_DISTRIBUTOROBJECT_H
