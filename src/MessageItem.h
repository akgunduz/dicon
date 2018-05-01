//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_MESSAGEITEM_H
#define BANKOR_MESSAGEITEM_H


#include "Message.h"
#include "MessageDirection.h"
#include "SchedulerItem.h"

class MessageItem : public SchedulerItem {

public:

    long address;
    Message *msg;

public:

    MessageItem(MESSAGE_DIRECTION, long = 0, Message * = nullptr);

    long getAddress();
    Message* getMessage();
};


#endif //BANKOR_MESSAGEITEM_H
