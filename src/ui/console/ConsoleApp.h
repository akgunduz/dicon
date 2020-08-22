//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_CONSOLEAPP_H
#define DICON_CONSOLEAPP_H

#include "Application.h"

class ConsoleApp : public App {

public:
    ConsoleApp(int *, LOGLEVEL, std::vector<int>&);

    bool distPollHandler();
    bool distStateHandler();

    bool collStateHandler(long);
    bool collLoadJobHandler(long);
    bool collProcessHandler(long);

    bool nodeStateHandler(long);

    int run() override;

    int notifyHandler(COMPONENT, NOTIFYSTATE) override;
};

#endif //DICON_CONSOLEAPP_H
