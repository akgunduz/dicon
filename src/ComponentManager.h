//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_COMPONENTMANAGER_H
#define DICON_COMPONENTMANAGER_H

#include "Common.h"
#include "ComponentObject.h"

typedef std::map<long, ComponentObject*> TypeComponentList;

class ComponentManager {

    int idCounter;

protected:

    std::mutex mutex;

    TypeComponentList components;

    virtual void setObject(int, long) = 0;

public:

    ComponentManager();
    virtual ~ComponentManager();

    int getFreeID();

    void clear();

    int getID(long);
    ComponentObject* get(long);

    int add(long);



};


#endif //DICON_COMPONENTMANAGER_H
