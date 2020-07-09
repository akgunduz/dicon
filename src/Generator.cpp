//
// Created by akgun on 9.07.2020.
//

#include <algorithm>
#include "Util.h"

#define SOURCE_FILE "../data/html/index.html"
#define GENERATE_ROOT "../src/gen"
#define TARGET_FILE GENERATE_ROOT "/WebPage.cpp"

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

    Util::removePath(GENERATE_ROOT);

    mkdir(GENERATE_ROOT, S_IRWXU);

    char line [200];
    char generated [256];
    FILE *source = fopen(SOURCE_FILE, "r");
    if (source == nullptr) {
        PRINT("Index.html file could not opened");
        return 0;
    }

    FILE *target = fopen(TARGET_FILE, "w");
    if (target == nullptr) {
        PRINT("MainPage.cpp file could not opened");
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

    printf("Hello World!\n");
}
