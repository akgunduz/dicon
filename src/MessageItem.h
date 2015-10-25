//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_MESSAGECAPSULE_H
#define BANKOR_MESSAGECAPSULE_H


#include "Message.h"
#include "MessageDirection.h"
#include "SchedulerItem.h"

class MessageItem : public SchedulerItem {

public:

    long address;
    Message *msg;

    MessageItem(MESSAGE_DIRECTION type, long address, Message *msg) : SchedulerItem(type) {
        this->address = address;
        this->msg = msg;
    }
};


#endif //BANKOR_MESSAGECAPSULE_H
