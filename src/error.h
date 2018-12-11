//
// Created by kaiser on 18-12-10.
//

#ifndef TINY_C_COMPILER_ERROR_H
#define TINY_C_COMPILER_ERROR_H

#include "location.h"
#include "token.h"

#include <fmt/core.h>

#include <string>
#include <iostream>
#include <cstdlib>

namespace tcc {

void ErrorReportAndExit(const std::string &msg);

void ErrorReportAndExit(const SourceLocation &location, const std::string &msg);

void ErrorReportAndExit(const SourceLocation &location, TokenValue expect, TokenValue actual);

template<typename... Args>
void ErrorReportAndExit(const SourceLocation &location, const std::string &format_str, const Args &... args) {
    std::cerr << "At: " << location.ToString() << '\n';
    fmt::print(format_str, args...);
    std::exit(EXIT_FAILURE);
}

}

#endif //TINY_C_COMPILER_ERROR_H
