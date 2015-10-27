//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ContentItem.h"
#include "Util.h"
#include "Rule.h"

ContentItem::ContentItem() {

	mIsValid = false;

}

bool ContentItem::isValid() {

	return mIsValid;

}

void ContentItem::setValid(bool valid) {

	mIsValid = valid;

}
