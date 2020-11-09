//
// Created by Haluk AKGUNDUZ on 23.10.2015.
// Copyright (c) 2015 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_MESSAGEITEM_H
#define DICON_MESSAGEITEM_H

#include "Message.h"

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

#endif //DICON_MESSAGEITEM_H
