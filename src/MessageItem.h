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

    MessageItem(MESSAGE_DIRECTION type, long address = 0, Message *msg = nullptr) : SchedulerItem(type) {
        this->address = address;
        if (msg != nullptr) {
            this->msg = msg;
            this->priority = msg->getPriority();
        }
    }
};


#endif //BANKOR_MESSAGECAPSULE_H
