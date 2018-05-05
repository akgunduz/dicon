//
// Created by Haluk AKGUNDUZ on 03.05.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_APPLICATION_H
#define BANKOR_APPLICATION_H

#include "Common.h"
#include "DeviceList.h"
#include "Distributor.h"
#include "Collector.h"

enum class APPMODE {

    APPMODE_CONSOLE,
    APPMODE_WXWIDGETS
};

class Application {

    bool initStart;

public:

    Application(int, int, bool = true);

    void setStartState(bool);
    bool getStartState();

};


#endif //BANKOR_APPLICATION_H
