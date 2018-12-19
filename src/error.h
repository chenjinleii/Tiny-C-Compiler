//
// Created by kaiser on 18-12-10.
//

#ifndef TINY_C_COMPILER_ERROR_H
#define TINY_C_COMPILER_ERROR_H

#include "location.h"
#include "token.h"

#include <fmt/core.h>

#include <cstdlib>
#include <iostream>
#include <string>

namespace tcc {

void ErrorReportAndExit(const std::string &msg);

void ErrorReportAndExit(const SourceLocation &location, const std::string &msg);

void ErrorReportAndExit(const SourceLocation &location, TokenValue expect,
                        TokenValue actually);

template <typename... Args>
void ErrorReportAndExit(const SourceLocation &location,
                        const std::string &format_str, const Args &... args) {
  std::cerr << "At: " << location.ToString() << '\n';
  fmt::print(format_str, args...);
  std::exit(EXIT_FAILURE);
}

template <typename... Args>
void ErrorReportAndExit(const std::string &format_str, const Args &... args) {
  fmt::print(format_str, args...);
  std::exit(EXIT_FAILURE);
}

}  // namespace tcc

#endif  // TINY_C_COMPILER_ERROR_H
