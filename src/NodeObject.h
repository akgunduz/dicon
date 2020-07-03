//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef DICON_NODEOBJECT_H
#define DICON_NODEOBJECT_H

#include "NodeState.h"
#include "Md5.h"
#include "ComponentObject.h"
#include "NodeProcessInfo.h"

class NodeObject : public ComponentObject {

private:

    NODESTATES state;

    int usage;

    NodeProcessInfo processInfo;

public:

    NodeObject(NODESTATES, int, int, long);
    NodeObject(int, long);
    NodeObject(const char*);
    NodeObject(const NodeObject &);
    NodeObject(const ComponentObject &);
    NodeObject();
    ~NodeObject();

    int getUsage();
    int iterateUsage(bool);

    NODESTATES getState();
    void setState(NODESTATES);

    NodeProcessInfo& getProcess();

};


#endif //DICON_NODEOBJECT_H
