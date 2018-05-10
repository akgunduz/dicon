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
};


#endif //BANKOR_UUID_H
