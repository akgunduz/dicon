//
// Created by akgunduz on 26.10.2015.
//

#ifndef DICON_JSONTYPE_H
#define DICON_JSONTYPE_H

#include "Common.h"

class JobItem;

typedef bool (*fParser)(JobItem*, json_object *);

class JsonType {

public:
    int id;
    char name[50];
    fParser parser;
    JobItem* parent;

    JsonType(int id, const char* name, JobItem *parent, fParser parser) {
        this->id = id;
        strcpy(this->name, name);
        this->parser = parser;
        this->parent = parent;
    }
};

typedef std::unique_ptr<JsonType> TypeJsonType;

#endif //DICON_JSONTYPE_H
