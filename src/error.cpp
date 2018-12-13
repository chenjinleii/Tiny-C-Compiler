//
// Created by kaiser on 18-12-10.
//

#include "error.h"

#include <cstdlib>

namespace tcc {

void ErrorReportAndExit(const std::string &msg) {
    std::cerr << msg << '\n';
    std::exit(EXIT_FAILURE);
}

void ErrorReportAndExit(const SourceLocation &location, const std::string &msg) {
    std::cerr << "At: " << location.ToString() << '\n';
    std::cerr << msg << '\n';
    std::exit(EXIT_FAILURE);
}

void ErrorReportAndExit(const SourceLocation &location, TokenValue expect, TokenValue actual) {
    std::cerr << "At: " << location.ToString() << '\n';
    std::cerr << "expect " << TokenTypes::ToString(expect) <<
              " but actual " << TokenTypes::ToString(actual) << '\n';
    std::exit(EXIT_FAILURE);
}

llvm::Value *ErrorReport(const std::string &msg) {
    std::cerr << msg << '\n';
    return nullptr;
}

}
