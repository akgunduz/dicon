//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_COMPONENTOBJECT_H
#define DICON_COMPONENTOBJECT_H

#include "Common.h"
#include "ComponentInfo.h"

class ComponentObject : public ComponentInfo {

private:

    const char *rootPath{};

    ComponentInfo assigned{};

protected:

public:

    explicit ComponentObject(COMPONENT = COMP_DISTRIBUTOR, long = 0, long = 0);
    ComponentObject(COMPONENT, const char*);
    ComponentObject(COMPONENT, const char*, long, long);
    ComponentObject(const ComponentObject&);
    ComponentObject(const ComponentObject&, COMPONENT);

    virtual ~ComponentObject();

    const char* getName() const;

    const char* getRootPath() const;
    void setRootPath(const char*);

    const ComponentInfo& getAssigned() const;
    void setAssigned(COMPONENT, long, long);
};

#endif //DICON_COMPONENTOBJECT_H
