//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_COMPONENTMANAGER_H
#define DICON_COMPONENTMANAGER_H

#include "Common.h"
#include "ComponentObject.h"

typedef std::map<int, ComponentObject*> TypeComponentMapList;
typedef std::vector<ComponentObject*> TypeComponentVectorList;

class ComponentManager {

    int idCounter;

    std::mutex mutex;

    TypeComponentMapList componentsMap;
    TypeComponentVectorList componentsIndex;

protected:

    virtual ComponentObject* createObject(int, long) = 0;

public:

    ComponentManager();
    virtual ~ComponentManager();

    ComponentObject* get(int);
    ComponentObject* getByIndex(int);
    size_t size();
    void clear();
    bool isExist(int);

    int add(long);
};


#endif //DICON_COMPONENTMANAGER_H
