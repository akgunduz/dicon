//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_FILEITEM_H
#define DICON_FILEITEM_H

#include "ContentItem.h"

class FileItem : public ContentItem {

	std::string name;

	bool is_exist{};
	bool is_required{};
	long size{};

public:
	explicit FileItem(const TypeHostUnit&, long = 0, long = 0, std::string = "");
	~FileItem() override = default;

    const std::string& getName() const;
    void setName(const std::string&);

    long getSize() const;

	CONTENT_TYPES getType() const override;

    bool check() override;

    bool required() const;
    void setRequired(bool);
};

typedef std::shared_ptr<FileItem> TypeFileItem;
typedef std::vector<TypeFileItem> TypeFileList;

#endif //DICON_FILEITEM_H
