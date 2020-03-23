//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_COMPONENTOBJECT_H
#define DICON_COMPONENTOBJECT_H

#include "Common.h"

enum COMPONENT {

    COMP_DISTRIBUTOR,
    COMP_COLLECTOR,
    COMP_NODE,
    COMP_MAX
};

class ComponentObject {

private:

    COMPONENT type;

    int id;

    const char *rootPath;

    long address;

protected:

public:

    ComponentObject(COMPONENT, int = 0);
    ComponentObject(COMPONENT, int, long);
    ComponentObject(COMPONENT, const char*);
    ComponentObject(COMPONENT, const char*, int, long);
    ComponentObject(const ComponentObject&);

    virtual ~ComponentObject();

    COMPONENT getType();
    int getID();
    void setID(int);
    const char* getName();

    const char* getRootPath();
    void setRootPath(const char*);

    long getAddress();
    void setAddress(long);

    COMPONENT getNextType();
    COMPONENT getPrevType();
};


#endif //DICON_COMPONENTOBJECT_H
