//
// Created by akgunduz on 26.10.2015.
//

#ifndef BANKOR_JSONTYPE_H
#define BANKOR_JSONTYPE_H

#include "Common.h"

class JsonItem;

typedef bool (*fParser)(JsonItem*, json_object *);

class JsonType {

public:
    int id;
    char name[50];
    fParser parser;
    JsonItem* parent;

    JsonType(int id, const char* name, JsonItem *parent, fParser parser) {
        this->id = id;
        strcpy(this->name, name);
        this->parser = parser;
        this->parent = parent;
    }
};


#endif //BANKOR_JSONTYPE_H
