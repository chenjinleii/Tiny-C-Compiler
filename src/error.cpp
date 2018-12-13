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
    std::cerr << "Grammatical errors: At: " << location.ToString() << '\n';
    std::cerr << "Expect " << TokenTypes::ToString(expect).substr(1) <<
              " but actual " << TokenTypes::ToString(actual).substr(1) << '\n';
    std::exit(EXIT_FAILURE);
}

}
