//
// Created by akgunduz on 23.10.2015.
//

#include "MessageItem.h"

MessageItem::MessageItem(MESSAGE_DIRECTION type, long address, Message *msg)
: SchedulerItem(type), address(address), msg(msg) {

    if (msg != nullptr) {
        priority = msg->getHeader().getPriority();
    }
}

long MessageItem::getAddress() {

    return address;
}

Message *MessageItem::getMessage() {

    return msg;
}
