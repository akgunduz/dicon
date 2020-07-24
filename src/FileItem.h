//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __FileContent_H_
#define __FileContent_H_

#include "ContentItem.h"

class FileItem : public ContentItem {

	char name[PATH_MAX]{};

	bool is_exist{};
	long size{};

public:
	explicit FileItem(const ComponentObject&, long = 0, long = 0, const char* = "");
	~FileItem() override = default;

    const char* getName() const;
    void setName(const char*);

    long getSize() const;

	CONTENT_TYPES getType() const override;

    bool check() override;
};

typedef std::vector<FileItem*> TypeFileList;

#endif //__Content_H_
