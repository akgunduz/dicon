//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_FILEITEM_H
#define DICON_FILEITEM_H

#include "ContentItem.h"

#define ARCH_PATH "bin"

class FileItem : public ContentItem {

	std::string name;
    std::filesystem::path parentPath;

    bool is_independent{false};
    bool is_required{};
    bool is_executable{};

    std::uintmax_t size[ARCH_MAX]{};

public:
	FileItem(const TypeHostUnit&, long, long = 0, std::string = "");
    explicit FileItem(const TypeHostUnit&, bool = false);
	FileItem(const TypeHostUnit&, long, std::filesystem::path, std::string);
	~FileItem() override = default;

    const std::string& getName();
    void setName(const std::string&);

    std::uintmax_t getSize(ARCH = ARCH_FREE);

	CONTENT_TYPES getType() override;

    bool checkContent() override;

    bool required() const;
    void setRequired(bool);

    bool isIndependent() const;
    void setIndependent(bool);

    bool isExecutable() const;
    void setExecutable(bool);

    std::filesystem::path getParentPath(ARCH, bool);
    std::filesystem::path getParentRefPath(ARCH, bool);
    std::filesystem::path getPath(ARCH = ARCH_FREE, bool = true);
    std::filesystem::path getRefPath(ARCH = ARCH_FREE, bool = true);
};

typedef std::shared_ptr<FileItem> TypeFileItem;
typedef std::vector<TypeFileItem> TypeFileList;

#endif //DICON_FILEITEM_H
