//
// Created by akgunduz on 26.10.2015.
//

#ifndef BANKOR_JSONFILE_H
#define BANKOR_JSONFILE_H


#include "FileContent.h"
#include "JsonType.h"

#define MAX_CONTENT_TYPE 10

class JsonFile {

    char rootPath[PATH_MAX];

protected:

    Unit unitHost;
    Unit unitNode;

    FileContent* content;
    std::vector<Content*> contentList[MAX_CONTENT_TYPE];

    std::map<int, JsonType *> contentTypes;

public:

    JsonFile(Unit unitHost, Unit unitNode, const char* path);
    virtual ~JsonFile();
    FileContent* getContent();
    const char* getRootPath();
    Content* getContent(int type, int index);
    int getContentCount(int type);
    int getFlaggedFileCount();
    void reset();
    bool parse(const char* path);

    virtual const char* getFileName() = 0;
    virtual FILETYPE getFileType() = 0;
};


#endif //BANKOR_JSONFILE_H
