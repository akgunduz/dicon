//
// Created by Haluk AKGUNDUZ on 30.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_USERINTERFACECONTROLLER_H
#define BANKOR_USERINTERFACECONTROLLER_H

#include "UserInterfaceEvent.h"

typedef void (*TypeUIUpdateCB)(void*, int, void*);

enum UI_UPDATE2 {

    UI_UPDATE_ADDRESS1,
    UI_UPDATE_ADDRESS2,
    UI_UPDATE_ADDRESS3,
    UI_UPDATE_LIST1,
    UI_UPDATE_LIST2,
    UI_UPDATE_ITEM1,
    UI_UPDATE_ITEM2,
    UI_UPDATE_STATE,
    UI_UPDATE_CLEAR,

    UI_UPDATE2_MAX
};

class UserInterfaceController {

private:

    void *uiContext;
    TypeUIUpdateCB uiUpdateCB;
    static UserInterfaceController *instance;

    UserInterfaceController(void *, TypeUIUpdateCB);

public:

    static UserInterfaceController* newInstance(void *, TypeUIUpdateCB);
    void updateUI(int, UserInterfaceEvent*);

    void display(int, std::vector<long>);
    void display(int, std::vector<long>, const char* , ...);
    void display(int, const char* , ...);
    void display(int, void*);
};


#endif //BANKOR_USERINTERFACECONTROLLER_H
