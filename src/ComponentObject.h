//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef BANKOR_COMPONENTOBJECT_H
#define BANKOR_COMPONENTOBJECT_H

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

protected:

public:

    ComponentObject(COMPONENT, const char* = NULL, int = 0);
    ComponentObject(const ComponentObject&);

    virtual ~ComponentObject();

    COMPONENT getType();
    int getID();
    void setID(int);
    const char* getName();
    const char* getRootPath();

    COMPONENT getNextType();
    COMPONENT getPrevType();
};


#endif //BANKOR_COMPONENTOBJECT_H
