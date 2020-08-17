//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_COMPONENTMANAGER_H
#define DICON_COMPONENTMANAGER_H

#include "Common.h"
#include "HostUnit.h"

#define ALIVE_INTERVAL 8
#define CHECK_INTERVAL ALIVE_INTERVAL + 2

typedef std::map<long, ComponentUnit*> TypeComponentMapIDList;
typedef std::map<Address, ComponentUnit*> TypeComponentMapAddressList;

class ComponentManager {

    HostUnit *host;

    long idCounter;

    volatile bool threadRun = true;

    std::mutex mutex;
    std::thread thread;

    TypeComponentMapIDList componentsMapID;
    TypeComponentList componentsMapDead;
    TypeComponentMapAddressList componentsMapAddress;

protected:

    virtual ComponentUnit* createUnit(ARCH, long, Address&) = 0;

public:

    ComponentManager(HostUnit *, bool);
    virtual ~ComponentManager();

    TypeComponentMapIDList& get();
    TypeComponentList& getDead();
    ComponentUnit* get(long);
    ComponentUnit* getDead(long index);
    size_t size();
    void clear();
    bool isExist(long);

    void process();
    long add(ARCH, Address&, bool&);
};

#endif //DICON_COMPONENTMANAGER_H
