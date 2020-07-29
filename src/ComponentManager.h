//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_COMPONENTMANAGER_H
#define DICON_COMPONENTMANAGER_H

#include "Common.h"
#include "ComponentObject.h"

typedef std::map<long, ComponentObject*> TypeComponentMapIDList;
typedef std::map<long, ComponentObject*> TypeComponentMapAddressList;
typedef std::vector<ComponentObject*> TypeComponentVectorList;

class ComponentManager {

    long idCounter;

    std::mutex mutex;

    TypeComponentMapIDList componentsMapID;
    TypeComponentMapAddressList componentsMapAddress;
    TypeComponentVectorList componentsIndex;

protected:

    virtual ComponentObject* createObject(long, long) = 0;

public:

    ComponentManager();
    virtual ~ComponentManager();

    ComponentObject* get(long);
    ComponentObject* getByIndex(int);
    size_t size();
    void clear();
    bool isExist(long);

    long add(long, bool&);
};


#endif //DICON_COMPONENTMANAGER_H
