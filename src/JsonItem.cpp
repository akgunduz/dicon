//
// Created by akgunduz on 26.10.2015.
//

#include "JsonItem.h"

JsonItem::JsonItem(COMPONENT host)
    : FileItem(host) {

}

JsonItem::JsonItem(const char* jobDir, FileItem *fileItem)
    : FileItem(jobDir, fileItem) {

}

JsonItem::JsonItem(COMPONENT host, const char* jobDir, const char* fileName, FILETYPE fileType)
        : FileItem(host, jobDir, fileName, fileType) {

}

JsonItem::~JsonItem() {

    for (std::map<int , JsonType *>::iterator i = contentTypes.begin(); i != contentTypes.end(); i++) {
        delete i->second;
    }

    for (int i = 0; i < MAX_CONTENT_TYPE; i++) {
        for (int j = 0; j < contentList[i].size(); j++) {
            ContentItem *item = contentList[i][j];
            delete item;
        }
    }
}

ContentItem *JsonItem::getContent(int type, int index) {
    return contentList[type][index];
}

int JsonItem::getContentCount(int type) {
    return (int) contentList[type].size();
}

int JsonItem::getFlaggedFileCount() {

    int count = 0;

    for (uint16_t i = 0; i < getContentCount(CONTENT_FILE); i++) {
        FileItem *content = (FileItem *)getContent(CONTENT_FILE, i);
        if (content->isFlaggedToSent()) {
            count++;
        }
    }

    return count;
}

void JsonItem::reset() {

    for (int i = 0; i < contentList->size(); i++) {
        contentList[i].clear();
    }
}

bool JsonItem::parse() {

    std::string test = Util::absPath(getHost(), getRefPath().c_str()).c_str();
    struct json_object* node = json_object_from_file(Util::absPath(getHost(), getRefPath().c_str()).c_str());
    if (node == NULL){
        LOG_E("Invalid JSON File");
        return false;
    }

    struct json_object* header = (struct json_object*)json_object_get_object(node)->head->v;

    json_object_object_foreach(header, key, val) {

        for (std::map<int , JsonType *>::iterator i = contentTypes.begin(); i != contentTypes.end(); i++) {

            JsonType* type = i->second;
            if (strcmp(type->name, key) == 0) {
                (type->parser)(type->parent, val);
                break;
            }
        }
    }

    return true;
}