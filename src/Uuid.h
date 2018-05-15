//
// Created by Haluk AKGUNDUZ on 10.05.2018.
//

#ifndef BANKOR_UUID_H
#define BANKOR_UUID_H


#include "Array.h"

class Uuid : public Array {

public:

    Uuid() : Array(sizeof(uuid_t)) {

        uuid_t tmp = {};
        uuid_generate(tmp);
        set(tmp);
    }

    Uuid(uint8_t val) : Array(sizeof(uuid_t)) {

        uuid_t tmp = {};
        memset(tmp, val, sizeof(uuid_t));
        set(tmp);
    }
};


#endif //BANKOR_UUID_H
