//
// Created by akgunduz on 26.10.2015.
//

#ifndef DICON_JSONTYPE_H
#define DICON_JSONTYPE_H

#include "Common.h"
#include <json.h>

class JobItem;

typedef bool (*fParser)(JobItem*, json_object *);

class JsonType {

public:
    int id;
    std::string name;
    fParser parser;
    JobItem* parent;

    JsonType(int _id, const std::string& _name, JobItem *_parent, fParser _parser) {
        id = _id;
        name = _name;
        parent = _parent;
        parser = _parser;
    }
};

typedef std::unique_ptr<JsonType> TypeJsonType;

#endif //DICON_JSONTYPE_H
