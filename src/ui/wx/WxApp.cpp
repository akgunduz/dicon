//
// Created by Haluk AKGUNDUZ on 24.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "WxApp.h"

WxApp::WxApp(int argc, char** argv, int *interfaceID,
                       LOGLEVEL* logLevel, int* distCount, int* collInfo, int* nodeInfo)
    : UserInterfaceApp(APPTYPE_WXWIDGETS, argc, argv, interfaceID, logLevel,
                       distCount, collInfo, nodeInfo) {

    app = new WxMain(componentController);
    WxMain::SetInstance((WxMain*)app);
}

int WxApp::run() {

    return wxEntry(argc, argv);
}
