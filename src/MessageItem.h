//
// Created by akgunduz on 23.10.2015.
//

#ifndef DICON_MESSAGEITEM_H
#define DICON_MESSAGEITEM_H

#include "Address.h"
#include "Message.h"
#include "MessageTypes.h"
#include "SchedulerItem.h"

class MessageItem : public SchedulerItem {

    CommUnit unit{};
    Message *msg{};

public:

    MessageItem(MSG_DIR, CommUnit&, Message * = nullptr);
    MessageItem(MSG_DIR);

    CommUnit& getUnit();
    Message* getMessage();
};


#endif //DICON_MESSAGEITEM_H
