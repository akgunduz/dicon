//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_CONTENTITEM_H
#define DICON_CONTENTITEM_H

#include "Log.h"
#include "SchedulerItem.h"

enum CONTENT_TYPES {
    CONTENT_NAME,
	CONTENT_FILE,
	CONTENT_PARAM,
	CONTENT_PROCESS,
    CONTENT_MAX
};

class ContentItem : public SchedulerItem {

    ComponentObject host;
    long id{};
    long assignedJob{};

public:
    explicit ContentItem(const ComponentObject&, long = 0, long = 0);
    ContentItem(const ContentItem&);

    virtual CONTENT_TYPES getType() const = 0;
    virtual bool check() = 0;
	virtual ~ContentItem() = default;

    const ComponentObject& getHost() const;

    long getID() const;

    virtual void setID(long);

    long getAssignedJob() const;
    void setAssignedJob(long);
};

bool compareContentID(ContentItem * ci1, ContentItem * ci2);

#endif //DICON_CONTENTITEM_H
