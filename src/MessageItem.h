//
// Created by akgunduz on 23.10.2015.
//

#ifndef DICON_MESSAGEITEM_H
#define DICON_MESSAGEITEM_H

#include "Address.h"
#include "Message.h"
#include "MessageType.h"
#include "SchedulerItem.h"

class MessageItem : public SchedulerItem {

    CommUnit unit{};
    TypeMessage msg{};

public:

    MessageItem(MSG_DIR, CommUnit&, TypeMessage = nullptr);
    ~MessageItem() override;

    CommUnit& getUnit();
    TypeMessage& getMessage();
};

typedef std::shared_ptr<MessageItem> TypeMessageItem;

#endif //DICON_MESSAGEITEM_H
