//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_CONSOLEAPP_H
#define DICON_CONSOLEAPP_H

#include "Application.h"

class ConsoleApp : public App {

public:
    ConsoleApp(int, char**, int *, LOGLEVEL*, int*);

    bool distPollHandler();
    bool distStateHandler();

    bool collStateHandler(int);
    bool collLoadJobHandler(int);
    bool collProcessHandler(int);

    bool nodeStateHandler(int);

    int run() override;

    int notifyHandler(int, int) override;
};


#endif //DICON_CONSOLEAPP_H
