//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_FILEITEM_H
#define DICON_FILEITEM_H

#include "ContentItem.h"

class FileItem : public ContentItem {

	std::string name;
    std::filesystem::path parentPath;

    bool is_independent{false};
public:
    bool isIndependent() const;

    void setIndependent(bool isIndependent);

private:
    bool is_exist{};
	bool is_required{};

    std::uintmax_t size{};

public:
	FileItem(const TypeHostUnit&, long, long = 0, std::string = "");
    explicit FileItem(const TypeHostUnit&, bool = false);
	FileItem(const TypeHostUnit&, long, std::filesystem::path, std::string);
	~FileItem() override = default;

    const std::string& getName();
    void setName(const std::string&);

    std::uintmax_t getSize();

	CONTENT_TYPES getType() override;

    bool check() override;

    bool required() const;
    void setRequired(bool);

    std::filesystem::path getParentPath();
    std::filesystem::path getParentRefPath();
    std::filesystem::path getPath();
    std::filesystem::path getRefPath();
};

typedef std::shared_ptr<FileItem> TypeFileItem;
typedef std::vector<TypeFileItem> TypeFileList;

#endif //DICON_FILEITEM_H
