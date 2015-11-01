//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ContentItem.h"
#include "Util.h"
#include "Rule.h"

ContentItem::ContentItem() {

	this->valid = false;
}

bool ContentItem::isValid() {

	return valid;

}

void ContentItem::setValid(bool valid) {

	this->valid = valid;

}