//
// Created by Haluk AKGUNDUZ on 03.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_WEBOPTION_H
#define DICON_WEBOPTION_H

#include <string>
#include <map>

class WebOption {

    const char* list[10]{};
    std::map<std::string, std::string> options;

public:
     const char** getOptions();
    void setOption(const std::string&, int);
    void setOption(const std::string&, std::string);

};

#endif //DICON_WEBOPTION_H
