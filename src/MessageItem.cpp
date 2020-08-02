//
// Created by akgunduz on 23.10.2015.
//

#include "MessageItem.h"

MessageItem::MessageItem(MSG_DIR _type)
        : SchedulerItem(_type) {
}

MessageItem::MessageItem(MSG_DIR _type, ComponentUnit& _unit, Message *_msg)
        : SchedulerItem(_type), unit(_unit), msg(_msg) {

    if (msg != nullptr) {
        priority = msg->getHeader().getPriority();
    }
}

ComponentUnit& MessageItem::getUnit() {

    return unit;
}

Message *MessageItem::getMessage() {

    return msg;
}
