//
// Created by Haluk AKGUNDUZ on 22/09/20.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_USERDATA_H
#define DICON_USERDATA_H

class UserData {

public:
    void *data{};
    UserData() = default;
    explicit UserData(void* _data) {
        data = _data;
    }
    virtual ~UserData() = default;
};


#endif //DICON_USERDATA_H
