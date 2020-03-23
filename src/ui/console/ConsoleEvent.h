//
// Created by Haluk AKGUNDUZ on 6.05.2018.
//

#ifndef DICON_CONSOLEEVENT_H
#define DICON_CONSOLEEVENT_H

class ConsoleEvent {

    int id;
    void *dataPointer;

public:

    ConsoleEvent();

    void* GetClientData();

    int GetId();
    void SetId(int);
    void SetClientData(void *);
};



#endif //DICON_CONSOLEEVENT_H
