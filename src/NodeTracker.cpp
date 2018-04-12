//
// Created by Haluk AKGUNDUZ on 11.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "NodeTracker.h"

bool NodeTracker::add(Md5 md5, NodeInfo node) {

    md5node[md5].push_back(node);
    nodeMd5[node].push_back(md5);

    return true;
}

void NodeTracker::clear() {

    md5node.clear();
    nodeMd5.clear();
}

bool NodeTracker::isNodeContains(NodeInfo node, Md5 md5) {

    TypeNodeMD5Map::iterator search = nodeMd5.find(node);
    if (search == nodeMd5.end()) {
        return false;
    }

    std::vector<Md5> md5List = search->second;
    for (int i = 0; i < md5List.size(); i++) {
        if (md5List[i].equal(&md5)) {
            return true;
        }
    }

    return false;
}





