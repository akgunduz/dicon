//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_CONTENTITEM_H
#define BANKOR_CONTENTITEM_H

#include "Log.h"
#include "SchedulerItem.h"

#define BUFFER_SIZE 512

enum CONTENT_TYPES {
    CONTENT_NAME,
	CONTENT_FILE,
	CONTENT_PARAM,
	CONTENT_PROCESS,
    CONTENT_MAX
};

class ContentItem : public SchedulerItem {

public:

	ContentItem() {}

	virtual CONTENT_TYPES getType() = 0;
    virtual bool isValid() = 0;
	virtual ~ContentItem(){}
};

#endif //BANKOR_CONTENTITEM_H
