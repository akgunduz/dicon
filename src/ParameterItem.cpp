//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ParameterItem.h"

ParameterItem::ParameterItem(const TypeHostUnit& host, long id, long jobID, const std::string& _param)
        : ContentItem (host, id, jobID) {

	param = _param;
};

CONTENT_TYPES ParameterItem::getType() {

	return CONTENT_PARAM;
}

const std::string& ParameterItem::get() {

    return param;
}

void ParameterItem::set(const std::string& _param) {

    param = _param;
}

bool ParameterItem::check() {

    return !param.empty();
}
