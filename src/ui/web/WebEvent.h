//
// Created by Haluk AKGUNDUZ on 23.06.2020.
//

#ifndef DICON_WEBEVENT_H
#define DICON_WEBEVENT_H

class WebEvent {

    int id;
    void *dataPointer;

public:

    WebEvent();

    void* GetClientData();

    int GetId();
    void SetId(int);
    void SetClientData(void *);
};



#endif //DICON_WEBEVENT_H
