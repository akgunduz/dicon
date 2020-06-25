//
// Created by Haluk AKGUNDUZ on 24.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_WXAPP_H
#define DICON_WXAPP_H

#include "Common.h"
#include "UserInterfaceApp.h"
#include "WxMain.h"

class WxApp : public UserInterfaceApp {

    WxMain *app;

public:
    WxApp(int argc, char** argv, int *interfaceID,
            LOGLEVEL* logLevel, int* distCount, int* collInfo, int* nodeInfo);

    int run() override;
};


#endif //DICON_WXAPP_H
