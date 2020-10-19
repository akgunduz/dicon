//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_CONSOLEAPP_H
#define DICON_CONSOLEAPP_H

#include <mutex>
#include <HostUnit.h>
#include <CommInterface.h>
#include "Application.h"
#include "Platform.h"

class ConsoleApp : public App {

public:
    ConsoleApp(AppParams&);

    bool distPollHandler();
    bool distStateHandler();

    bool collStateHandler(long);
    bool collLoadJobHandler(long);
    bool collProcessHandler(long);

    bool nodeStateHandler(long);

    int run() override;

    int notifyHandler(COMPONENT, NOTIFYTYPE) override;
};

#endif //DICON_CONSOLEAPP_H
