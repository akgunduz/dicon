//
// Created by akgunduz on 23.10.2015.
//

#include "MessageItem.h"

MessageItem::MessageItem(MSG_DIR _type, CommUnit& _unit, TypeMessage _msg)
        : SchedulerItem(_type), unit(_unit), msg(std::move(_msg)) {

    if (msg != nullptr) {
        priority = msg->getHeader().getPriority();
    }
}

CommUnit& MessageItem::getUnit() {

    return unit;
}

TypeMessage& MessageItem::getMessage() {

    return msg;
}

MessageItem::~MessageItem() = default;
