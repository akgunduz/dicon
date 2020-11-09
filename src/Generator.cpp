//
// Created by Haluk AKGUNDUZ on 09.07.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include <cstdarg>
#include <algorithm>
#include "Util.h"

//#define SOURCE_FILE "../data/html/index.html"
//#define TARGET_FILE "../src/gen/WebPage.cpp"

#define PRINT(a, ...) show(a, ##__VA_ARGS__)

void show(const char *format, ...) {

    char buf[256];
    va_list ap;
    va_start(ap, format);
    vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);
    printf("%s \n", buf);

}

int main(int argc, char** argv) {

    if (argc < 3) {

        PRINT("Please specify input html and output cpp files");
        return 0;
    }

    std::filesystem::path sourcePath = argv[1];
    std::filesystem::path targetPath = argv[2];

    if (!std::filesystem::exists(sourcePath)) {
        PRINT("Can not find source html file, exiting!!!");
        return 0;
    }

    if (std::filesystem::exists(targetPath.parent_path())) {
        std::filesystem::remove_all(targetPath.parent_path());
    }

    std::filesystem::create_directories(targetPath.parent_path());

    char line [200];
    char generated [256];
    FILE *source = fopen(sourcePath.string().c_str(), "r");
    if (source == nullptr) {
        PRINT("Index.html file could not opened");
        return 0;
    }

    FILE *target = fopen(targetPath.string().c_str(), "w");
    if (target == nullptr) {
        PRINT("WebPage.cpp file could not opened");
        return 0;
    }

    fputs("#include \"civetweb/civetweb.h\"\n", target);
    fputs("void sendHtml(struct mg_connection *conn) {\n", target);
    fputs("\tmg_printf(conn, \"HTTP/1.1 200 OK\\r\\nContent-Type: text/html\\r\\nConnection: close\\r\\n\\r\\n\");\n", target);

    while(fgets(line, 200, source)) {
        std::string strLine = line;
        strLine.erase(std::remove(strLine.begin(), strLine.end(), '\n'), strLine.end());
        strLine.erase(std::remove(strLine.begin(), strLine.end(), '\r'), strLine.end());
        Util::replaceStr(strLine, "\"", "\\\"");
        Util::replaceStr(strLine, "%", "%%");
        if (!strLine.empty()) {
            sprintf(generated, "\tmg_printf(conn, \"%s\\r\\n\");\n", strLine.c_str());
            fputs(generated, target);
        }
    }

    fputs("}\n", target);

    fclose(source);
    fclose(target);
}
