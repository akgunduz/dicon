//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_CONTENTITEM_H
#define DICON_CONTENTITEM_H

#include "SchedulerItem.h"
#include "HostUnit.h"

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
    bool is_checked{false};

protected:

    bool is_valid{false};

public:
    explicit ContentItem(const TypeHostUnit&, TypeID = 0, long = 0);
    ContentItem(const ContentItem&);

    virtual CONTENT_TYPES getType() = 0;
    virtual bool checkContent() = 0;
    bool check(bool = false);
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
