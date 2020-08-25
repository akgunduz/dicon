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

    const TypeHostUnit host;
    long id{};
    long assignedJob{};

public:
    explicit ContentItem(const TypeHostUnit&, long = 0, long = 0);
    ContentItem(const ContentItem&);

    virtual CONTENT_TYPES getType() = 0;
    virtual bool check() = 0;
	~ContentItem() override = default;

    const TypeHostUnit& getHost();

    long getID();

    virtual void setID(long);

    long getAssignedJob();
    void setAssignedJob(long);
};

typedef std::shared_ptr<ContentItem> TypeContentItem;
typedef std::vector<TypeContentItem> TypeContentList;

bool compareContentID(const TypeContentItem&, const TypeContentItem&);

#endif //DICON_CONTENTITEM_H
