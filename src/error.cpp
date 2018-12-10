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

void ErrorReportAndExit(TokenValue expect, TokenValue actual) {
    std::cerr << "expect " << TokenValues::ToString(expect) <<
              " bug actual " << TokenValues::ToString(actual) << '\n';
    std::exit(EXIT_FAILURE);
}

}
