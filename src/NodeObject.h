//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef BANKOR_NODEOBJECT_H
#define BANKOR_NODEOBJECT_H

#include "NodeState.h"
#include "Md5.h"

class NodeObject {

private:

    NODE_STATES state;
    int usage;
    int id;
    std::vector<Md5> md5List;

public:

    NodeObject(NODE_STATES, int, int);
    NodeObject(int);
    NodeObject();
    ~NodeObject();

    int getID();
    NODE_STATES getState();
    void setState(NODE_STATES);
    int getUsage();
    int iterateUsage(bool);
    std::vector<Md5>* getMD5List();

};


#endif //BANKOR_NODEOBJECT_H
