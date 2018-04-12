//
// Created by akgunduz on 26.10.2015.
//

#ifndef BANKOR_JSONFILE_H
#define BANKOR_JSONFILE_H


#include "FileItem.h"
#include "JsonType.h"
#include "SchedulerItem.h"

#define MAX_CONTENT_TYPE 10

class JsonItem : public FileItem {

protected:

    std::vector<ContentItem *> contentList[MAX_CONTENT_TYPE];

    std::map<int, JsonType *> contentTypes;

public:

    JsonItem(Unit host);
    JsonItem(const char*, FileItem *fileItem);
    JsonItem(Unit host, const char* jobDir,
             const char* fileName, FILETYPE fileType);
    virtual ~JsonItem();
    ContentItem * getContent(int type, int index);
    int getContentCount(int type);
    int getFlaggedFileCount();
    void reset();
    bool parse();
};


#endif //BANKOR_JSONFILE_H
