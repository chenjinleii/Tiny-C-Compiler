//
// Created by kaiser on 18-12-10.
//

#ifndef TINY_C_COMPILER_ERROR_H
#define TINY_C_COMPILER_ERROR_H

#include "token.h"

#include <string>

namespace tcc {

void ErrorReportAndExit(const std::string &msg);
void ErrorReport(const SourceLocation &location, const std::string &msg);

}

#endif //TINY_C_COMPILER_ERROR_H
