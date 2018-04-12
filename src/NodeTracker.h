//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_NODETRACKER_H
#define BANKOR_NODETRACKER_H

#include "Common.h"
#include "Md5.h"
#include "NodeInfo.h"

typedef std::map<Md5, std::vector<NodeInfo>, cmp_md5> TypeMD5NodeMap;
typedef std::map<NodeInfo, std::vector<Md5>, cmp_node> TypeNodeMD5Map;

class NodeTracker {

    TypeMD5NodeMap md5node;
    TypeNodeMD5Map nodeMd5;

public:

    bool add(Md5, NodeInfo);
    bool isNodeContains(NodeInfo, Md5);
//    bool remove(Md5);
//    bool remove(NodeInfo);
//    Md5 get(NodeInfo);
//    NodeInfo get(Md5);
    void clear();

};


#endif //BANKOR_NODETRACKER_H
