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
    uint64_t duration{};

public:
    explicit ContentItem(const TypeHostUnit&, TypeID = 0, long = 0);
    ContentItem(const ContentItem&);

    virtual CONTENT_TYPES getType() = 0;
    virtual bool check() = 0;
	~ContentItem() override = default;

    const TypeHostUnit& getHost();

    TypeID getID();

    virtual void setID(TypeID);

    long getAssignedJob();
    void setAssignedJob(long);

    uint64_t getDuration() const;

    void setDuration(uint64_t duration);
};

typedef std::shared_ptr<ContentItem> TypeContentItem;
typedef std::vector<TypeContentItem> TypeContentList;

bool compareContentID(const TypeContentItem&, const TypeContentItem&);

#endif //DICON_CONTENTITEM_H
