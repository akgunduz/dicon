//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_COMPONENTMANAGER_H
#define DICON_COMPONENTMANAGER_H

#include "Common.h"
#include "HostUnit.h"

typedef std::map<long, ComponentUnit*> TypeComponentMapIDList;
typedef std::map<Address, ComponentUnit*> TypeComponentMapAddressList;
typedef std::vector<ComponentUnit*> TypeComponentVectorList;

class ComponentManager {

    long idCounter;

    std::mutex mutex;

    TypeComponentMapIDList componentsMapID;
    TypeComponentMapAddressList componentsMapAddress;
    TypeComponentVectorList componentsIndex;

protected:

    virtual ComponentUnit* createUnit(long, Address&) = 0;

public:

    ComponentManager();
    virtual ~ComponentManager();

    ComponentUnit* get(long);
    ComponentUnit* getByIndex(int);
    size_t size();
    void clear();
    bool isExist(long);

    long add(Address&, bool&);
};


#endif //DICON_COMPONENTMANAGER_H
