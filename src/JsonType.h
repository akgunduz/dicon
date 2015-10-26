//
// Created by akgunduz on 26.10.2015.
//

#ifndef BANKOR_JSONTYPE_H
#define BANKOR_JSONTYPE_H

#include "Common.h"

typedef bool (*fParser)(void*, json_object *);

class JsonType {

public:
    int id;
    char name[50];
    fParser parser;
    void* parent;

    JsonType(int id, const char* name, void *parent, fParser parser) {
        this->id = id;
        strcpy(this->name, name);
        this->parser = parser;
        this->parent = parent;
    }
};


#endif //BANKOR_JSONTYPE_H
