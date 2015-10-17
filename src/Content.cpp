//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Content.h"
#include "Util.h"
#include "Rule.h"

Content::Content() {

	mIsValid = false;

}

bool Content::isValid() {

	return mIsValid;

}

void Content::setValid(bool valid) {

	mIsValid = valid;

}
