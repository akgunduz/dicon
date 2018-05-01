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

    static char rootPath[COMP_MAX][PATH_MAX];

private:

    COMPONENT type;

    int id;

protected:

public:

    ComponentObject(COMPONENT, int = 0);
    ComponentObject(const ComponentObject&);

    virtual ~ComponentObject();

    COMPONENT getType();
    int getID();
    void setID(int);
    const char* getName();

    COMPONENT getNextType();
    COMPONENT getPrevType();

    const char* getRootPath();
    static void setRootPath(ComponentObject, const char*);

};


#endif //BANKOR_COMPONENTOBJECT_H
