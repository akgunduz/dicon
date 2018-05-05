//
// Created by akgunduz on 26.10.2015.
//

#include "JsonItem.h"

JsonItem::JsonItem(ComponentObject host)
    : FileItem(host) {

}

JsonItem::JsonItem(FileItem *fileItem)
    : FileItem(fileItem) {

}

JsonItem::JsonItem(ComponentObject host, const char* jobDir, const char* fileName)
        : FileItem(host, jobDir, fileName, -1) {

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

void JsonItem::reset() {

    for (int i = 0; i < contentList->size(); i++) {
        contentList[i].clear();
    }
}

bool JsonItem::parse() {

    struct json_object* node = json_object_from_file(Util::getAbsRefPath(getHost().getRootPath(), getJobDir(), getFileName()).c_str());
    if (node == NULL){
        LOGS_E(getHost(), "Invalid JSON File");
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