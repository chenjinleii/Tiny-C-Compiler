//
// Created by kaiser on 18-12-10.
//

#ifndef TINY_C_COMPILER_ERROR_H
#define TINY_C_COMPILER_ERROR_H

#include "token.h"

#include <fmt/core.h>

#include <string>
#include <iostream>

namespace tcc {

void ErrorReportAndExit(const std::string &msg);
void ErrorReport(const SourceLocation &location, const std::string &msg);

template<typename... Args>
void ErrorReport(const SourceLocation &location, const std::string &format_str, const Args &... args) {
    std::cerr << "At: " << location.ToString() << '\n';
    fmt::print(format_str, args...);
}

}

#endif //TINY_C_COMPILER_ERROR_H
