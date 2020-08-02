//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#ifndef DICON_COLLECTORHOST_H
#define DICON_COLLECTORHOST_H

#include "HostUnit.h"
#include "CollectorObject.h"

class CollectorHost : public HostUnit, public CollectorObject {

public:
    CollectorHost(COLLSTATES, long);
    explicit CollectorHost(long);
    explicit CollectorHost(const char*);
    CollectorHost();
    CollectorHost(const CollectorHost &);

    ~CollectorHost() override;

};


#endif //DICON_COLLECTORHOST_H
