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

    ComponentObject();
    ComponentObject(COMPONENT, int = 0);
    ComponentObject(COMPONENT, int, long);
    ComponentObject(COMPONENT, const char*);
    ComponentObject(COMPONENT, const char*, int, long);
    ComponentObject(const ComponentObject&);

    virtual ~ComponentObject();

    const COMPONENT getType() const;
    const int getID() const;
    void setID(int);
    const char* getName() const;

    const char* getRootPath() const;
    void setRootPath(const char*);

    const long getAddress() const;
    void setAddress(long);
};


#endif //DICON_COMPONENTOBJECT_H
