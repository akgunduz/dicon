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

    ComponentUnit unit{};
    Message *msg{};

public:

    MessageItem(MSG_DIR, ComponentUnit&, Message * = nullptr);
    MessageItem(MSG_DIR);

    ComponentUnit& getUnit();
    Message* getMessage();
};


#endif //DICON_MESSAGEITEM_H
