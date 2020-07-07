//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_COMPONENTOBJECT_H
#define DICON_COMPONENTOBJECT_H

#include "Common.h"
#include "ComponentInfo.h"

enum COMPONENT {

    COMP_DISTRIBUTOR,
    COMP_COLLECTOR,
    COMP_NODE,
    COMP_MAX
};

class ComponentObject : public ComponentInfo {

private:

    COMPONENT type;

    const char *rootPath;

    ComponentInfo assigned;

protected:

public:

    ComponentObject();
    ComponentObject(COMPONENT, int = 0);
    ComponentObject(COMPONENT, int, long);
    ComponentObject(COMPONENT, const char*);
    ComponentObject(COMPONENT, const char*, int, long);
    ComponentObject(const ComponentObject&);

    virtual ~ComponentObject();

    COMPONENT getType() const;

    const char* getName() const;

    const char* getRootPath() const;
    void setRootPath(const char*);

    const ComponentInfo& getAssigned() const;
    void setAssigned(int, long);

};


#endif //DICON_COMPONENTOBJECT_H
