//
// Created by Haluk AKGUNDUZ on 01.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
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

typedef std::shared_ptr<DistributorHost> TypeDistributorHost;

#endif //DICON_DISTRIBUTORHOST_H
