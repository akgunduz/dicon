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

    TypeCommUnit unit;
    TypeMessage msg;

public:

    MessageItem(MSG_DIR, const TypeCommUnit&, TypeMessage = nullptr);
    ~MessageItem() override;

    const TypeCommUnit& getUnit();
    TypeMessage& getMessage();
};

typedef std::shared_ptr<MessageItem> TypeMessageItem;
//typedef std::map<void*, TypeMessageItem> TypeMessageItemMap;

#endif //DICON_MESSAGEITEM_H
