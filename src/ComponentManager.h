//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_COMPONENTMANAGER_H
#define DICON_COMPONENTMANAGER_H

#include "Common.h"
#include "HostUnit.h"
#include "ComponentUnit.h"

#define WAKEUP_INTERVAL 5
#define ALIVE_INTERVAL (WAKEUP_INTERVAL + 2)
#define CHECK_INTERVAL (ALIVE_INTERVAL + 2)

class ComponentManager {

    const TypeHostUnit& host;

    TypeID idCounter;

    bool protect{false};

    volatile bool threadRun{true};

    std::mutex mutex;
    std::thread thread;

    TypeComponentMapIDList components;
    TypeComponentMapIDList componentsDead;
    TypeIDList deadList;

    void checkDead();

protected:

    virtual TypeComponentUnit createUnit(ARCH, TypeID, TypeAddress&) = 0;

public:

    ComponentManager(const TypeHostUnit&, bool);
    virtual ~ComponentManager();

    void updateDead();
    TypeComponentMapIDList& get(bool = false);
    TypeComponentUnit get(TypeID);
    void clear();

    void process();
    TypeID add(ARCH, TypeAddress&, bool&);
};

#endif //DICON_COMPONENTMANAGER_H
