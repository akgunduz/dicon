//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __Content_H_
#define __Content_H_

#include "Log.h"
#include "SchedulerItem.h"
#include "Unit.h"

#define BUFFER_SIZE 512

enum CONTENT_TYPES {
    CONTENT_ID,
    CONTENT_RUNTYPE,
    CONTENT_NAME,
	CONTENT_FILE,
	CONTENT_PARAM,
	CONTENT_EXECUTOR,
    CONTENT_MAX
};

class ContentItem : public SchedulerItem {

	bool valid;
public:
	bool isValid();
	void setValid(bool);
	ContentItem();

	virtual CONTENT_TYPES getType() = 0;
	virtual ~ContentItem(){};
};

#endif //__Content_H_
