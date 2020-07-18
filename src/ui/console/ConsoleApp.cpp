//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ConsoleApp.h"

ConsoleApp::ConsoleApp(int argc, char** argv, int *interfaceID,
                       LOGLEVEL* logLevel, int* componentCount)
    : App(APPTYPE_CONSOLE, argc, argv, interfaceID, logLevel, componentCount) {

    PRINT("Commands : ");
    PRINT("\tPoll     : 'p' ==> Execute polling command");
    PRINT("\tLoad job : 'l' ==> Execute load job command");
    PRINT("\tProcess  : 'x' ==> Execute process command");
    PRINT("\tQuit     : 'q' ==> Exit program");
}

int ConsoleApp::run() {

    int in;
    do {

        in = getchar();
        switch(in) {
            case 'p':
                distPollHandler();
                break;
            case 'l':
                collLoadJobHandler(1);
                break;
            case 'x':
                collProcessHandler(1);
                break;
            case 'q':
                return 0;
            default:
                break;
        }

    } while(true);
}


int ConsoleApp::notifyHandler(int target, int id) {

    switch(target) {
        case COMP_DISTRIBUTOR:
            distStateHandler();
            break;
        case COMP_COLLECTOR:
            collStateHandler(id);
            break;
        case COMP_NODE:
            nodeStateHandler(id);
            break;
        default:
            break;
    }

    return 0;
}

