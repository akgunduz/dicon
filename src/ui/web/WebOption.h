//
// Created by Haluk AKGUNDUZ on 3.08.2020.
//

#ifndef DICON_WEBOPTION_H
#define DICON_WEBOPTION_H

#include "Common.h"

class WebOption {

    const char* list[10]{};
    std::map<std::string, std::string> options;

public:
     const char** getOptions();
    void setOption(std::string, int);
    void setOption(std::string, std::string);

};

#endif //DICON_WEBOPTION_H
