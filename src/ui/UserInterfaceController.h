//
// Created by Haluk AKGUNDUZ on 30.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_USERINTERFACECONTROLLER_H
#define DICON_USERINTERFACECONTROLLER_H

#include "UserInterfaceEvent.h"

typedef void (*TypeUIUpdateCB)(void*, int, void*);

enum UI_UPDATE {

    UI_UPDATE_DIST,
    UI_UPDATE_COLL,
    UI_UPDATE_NODE,
    UI_UPDATE_MAX
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
    void display(int, int, const char* , const char*);
    void display(int, int, const char* , long);
    void display(int, void*);
};


#endif //DICON_USERINTERFACECONTROLLER_H
