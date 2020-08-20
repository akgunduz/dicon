//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_FILEITEM_H
#define DICON_FILEITEM_H

#include "ContentItem.h"

class FileItem : public ContentItem {

	char name[PATH_MAX]{};

	bool is_exist{};
	bool is_required{};
	long size{};

public:
	explicit FileItem(const HostUnit&, long = 0, long = 0, const char* = "");
	~FileItem() override = default;

    const char* getName() const;
    void setName(const char*);

    long getSize() const;

	CONTENT_TYPES getType() const override;

    bool check() override;

    bool required();
    void setRequired(bool);
};

typedef std::shared_ptr<FileItem> TypeFileItem;
typedef std::vector<TypeFileItem> TypeFileList;

#endif //DICON_FILEITEM_H
