//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#ifndef DICON_DISTRIBUTORHOST_H
#define DICON_DISTRIBUTORHOST_H

#include "HostUnit.h"
#include "DistributorObject.h"

class DistributorHost : public HostUnit, public DistributorObject  {

public:
    explicit DistributorHost(long = 0);
    explicit DistributorHost(const char*);
    DistributorHost(const DistributorHost &);

    ~DistributorHost() override;
};

#endif //DICON_DISTRIBUTORHOST_H
