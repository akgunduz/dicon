//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#include "DistributorHost.h"

DistributorHost::DistributorHost(long _id)
        : HostUnit(COMP_DISTRIBUTOR, _id) {
}

DistributorHost::DistributorHost(const char* rootPath) :
        HostUnit(COMP_DISTRIBUTOR, rootPath)  {
}

DistributorHost::DistributorHost(const DistributorHost &copy) = default;

DistributorHost::~DistributorHost() = default;