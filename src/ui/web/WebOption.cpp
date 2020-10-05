//
// Created by Haluk AKGUNDUZ on 3.08.2020.
//

#include "WebOption.h"

const char **WebOption::getOptions() {

    int i = 0;
    for (auto& opt : options ) {
        list[i++] = opt.first.c_str();
        list[i++] = opt.second.c_str();
    }
    list[i] = nullptr;
    return list;
}

void WebOption::setOption(const std::string& key, int value) {
    options[key] = std::to_string(value);
};

void WebOption::setOption(const std::string& key, std::string value) {
    options[key] = std::move(value);
};
