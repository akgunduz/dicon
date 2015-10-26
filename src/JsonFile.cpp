//
// Created by akgunduz on 26.10.2015.
//

#include "JsonFile.h"

JsonFile::JsonFile(Unit unitHost, Unit unitNode, const char *path) {

    this->unitHost = unitHost;
    this->unitNode = unitNode;
    strcpy(this->rootPath, path);

}

JsonFile::~JsonFile() {

    if (content != nullptr) {
        delete content;
        content = nullptr;
    }

    for (std::map<int , JsonType *>::iterator i = contentTypes.begin(); i != contentTypes.end(); i++) {
        delete i->second;
    }

    for (int i = 0; i < MAX_CONTENT_TYPE; i++) {
        for (int j = 0; j < contentList[i].size(); j++) {
            Content *item = contentList[i][j];
            delete item;
        }
    }
}


FileContent *JsonFile::getContent() {
    return content;
}

const char *JsonFile::getRootPath() {
    return rootPath;
}

Content *JsonFile::getContent(int type, int index) {
    return contentList[type][index];
}

int JsonFile::getContentCount(int type) {
    return (int) contentList[type].size();
}

int JsonFile::getFlaggedFileCount() {

    int count = 0;

    for (uint16_t i = 0; i < getContentCount(CONTENT_FILE); i++) {
        FileContent *content = (FileContent *)getContent(CONTENT_FILE, i);
        if (content->isFlaggedToSent()) {
            count++;
        }
    }

    return count;
}

void JsonFile::reset() {

    for (int i = 0; i < contentList->size(); i++) {
        contentList[i].clear();
    }
}

bool JsonFile::parse(const char *path) {

    struct json_object* node = json_object_from_file(path);
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