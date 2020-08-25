//
// Created by Haluk AKGUNDUZ on 02/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_UTIL_H
#define DICON_UTIL_H

#include "Common.h"

#define BUFFER_SIZE 512

#define ROOT_SIGN "@@@root@@@"

class Util {

public:

	static std::string hex2str(const uint8_t *, size_t);
	static bool str2hex(uint8_t *, const char *, uint32_t);
    static void replaceStr(std::string&, const std::string&, const std::string&);
    static std::string parsePath(const std::filesystem::path&, const std::string &);

    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    }

    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    static std::string to_narrow( const wchar_t *s, char dfault = '?',
                          const std::locale& loc = std::locale::classic())
    {
        std::ostringstream stm;

        while( *s != L'\0' ) {
            stm << std::use_facet< std::ctype<wchar_t> >( loc ).narrow( *s++, dfault );
        }
        return stm.str();
    }
};

#endif //DICON_UTIL_H
