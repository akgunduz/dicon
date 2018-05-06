//
// Created by Haluk AKGUNDUZ on 6.05.2018.
//

#ifndef BANKOR_CONSOLEEVENT_H
#define BANKOR_CONSOLEEVENT_H

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



#endif //BANKOR_CONSOLEEVENT_H
