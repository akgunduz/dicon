//
// Created by Haluk AKGUNDUZ on 23.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_NOTIFYTYPE_H
#define DICON_NOTIFYTYPE_H

enum NOTIFYTYPE {
    NOTIFYTYPE_PASSIVE,
    NOTIFYTYPE_ACTIVE,
    NOTIFYTYPE_ONCE,
    NOTIFYTYPE_TRANSPARENT,
    NOTIFYTYPE_MAX,
};

class NotifyType {

    static constexpr const char* sNotifyTypes[NOTIFYTYPE_MAX] = {
            "PASSIVE",
            "ACTIVE",
            "ONCE",
            "TRANSPARENT",
    };

public:

    static const char* getName(NOTIFYTYPE _type) {

        return sNotifyTypes[_type];
    }
};


#endif //DICON_NOTIFYTYPE_H
