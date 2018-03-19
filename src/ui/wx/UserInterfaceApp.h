//
// Created by Haluk AKGUNDUZ on 27/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __USERINTERFACEAPP_H_
#define __USERINTERFACEAPP_H_

#include "UserInterface.h"

class UserInterfaceApp : public wxApp {

    DECLARE_CLASS( UserInterfaceApp )
    DECLARE_EVENT_TABLE()

public:

    /// Constructor
    UserInterfaceApp();

    void Init();

    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();
};

DECLARE_APP(UserInterfaceApp)

#endif
