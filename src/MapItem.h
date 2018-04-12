//
// Created by akgunduz on 02.11.2015.
//

#ifndef BANKOR_MAPITEM_H
#define BANKOR_MAPITEM_H


#include "FileItem.h"
#include "ArchTypes.h"

class MapItem : public ContentItem {

    FILETYPE fileType;
    FileItem *files[ARCH_MAX + 1];
    int fileCount;

public:
    MapItem(Unit host, const char *jobDir, const char *fileName,
                       FILETYPE fileType);
    FileItem *get();
    FileItem *get(int);
    FileItem *get(ARCH);
    int getCount();
    FILETYPE getFileType();

    virtual CONTENT_TYPES getType();

    virtual bool isValid();
};

typedef std::vector<MapItem*> TypeMapList;

#endif //BANKOR_MAPITEM_H
