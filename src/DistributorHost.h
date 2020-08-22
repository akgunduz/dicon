//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#ifndef DICON_DISTRIBUTORHOST_H
#define DICON_DISTRIBUTORHOST_H

#include "HostUnit.h"
#include "DistributorObject.h"

class DistributorHost : public HostUnit, public DistributorObject  {

public:
    DistributorHost();

    DistributorHost(const DistributorHost &);

    ~DistributorHost() override;
};

typedef std::unique_ptr<DistributorHost> TypeDistributorHost;

#endif //DICON_DISTRIBUTORHOST_H
