//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#ifndef DICON_COLLECTORHOST_H
#define DICON_COLLECTORHOST_H

#include "HostUnit.h"
#include "CollectorObject.h"

class CollectorHost : public HostUnit, public CollectorObject {

public:
    explicit CollectorHost(long = 0, COLLSTATES = COLLSTATE_IDLE);

    CollectorHost(const CollectorHost &);

    ~CollectorHost() override;
};

typedef std::unique_ptr<CollectorHost> TypeCollectorHost;

#endif //DICON_COLLECTORHOST_H
