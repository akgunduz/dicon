//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef DICON_NODEOBJECT_H
#define DICON_NODEOBJECT_H

#include "NodeState.h"
#include "Md5.h"
#include "ComponentObject.h"
#include "ProcessInfo.h"

class NodeObject : public ComponentObject {

private:

    NODESTATES state;

    int usage;

    ProcessInfo processInfo;

public:

    NodeObject(NODESTATES, int, int, long);
    NodeObject(int, long);
    NodeObject(const char*);
    NodeObject();
    NodeObject(const NodeObject &);
    NodeObject(const ComponentObject &);

    ~NodeObject() override;

    int getUsage() const;
    int iterateUsage(bool);

    NODESTATES getState();
    void setState(NODESTATES);

    ProcessInfo& getProcessInfo();

};


#endif //DICON_NODEOBJECT_H
