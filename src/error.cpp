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

void ErrorReportAndExit(const SourceLocation &location, TokenValue expect, TokenValue actually) {
    std::cerr << "Grammatical errors: At: " << location.ToString() << '\n';
    std::cerr << "Expect " << TokenTypes::ToString(expect) <<
              " but actually " << TokenTypes::ToString(actually) << '\n';
    std::exit(EXIT_FAILURE);
}

}
