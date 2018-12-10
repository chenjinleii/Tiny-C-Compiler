//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_SCANNER_H
#define TINY_C_COMPILER_SCANNER_H

#include "token.h"

#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <utility>

namespace tcc {

class KeywordsDictionary {
public:
    KeywordsDictionary();
    const std::pair<TokenValue, std::int32_t> &Find(const std::string &name);
private:
    std::unordered_map<std::string, std::pair<TokenValue, std::int32_t>> keywords_;
};

class Scanner {
public:
    explicit Scanner(const std::string &file_name);
    std::vector<Token> Scan();
private:
    Token GetNextToken();

    Token HandleIdentifierOrKeyword();
    Token HandleNumber();
    Token HandleChar();
    Token HandleString();
    Token HandleEscape();
    Token HandleOctEscape();
    Token HandleHexEscape();

    void SkipSpace();
    void SkipComment();

    char GetNextChar();
    char PeekNextChar() const;
    std::pair<char, char> PeekNextTwoChar() const;
    void PutBack();
    bool HasNextChar() const;
    bool Try(char ch);
    bool Test(char ch) const;

    Token MakeToken(TokenValue value, const std::string &name);
    Token MakeToken(TokenValue value, std::int32_t precedence, const std::string &name);
    Token MakeToken(TokenValue value, const std::string &name, char char_value);
    Token MakeToken(TokenValue value, const std::string &name, std::int32_t int32_value);
    Token MakeToken(TokenValue value, const std::string &name, double double_value);
    Token MakeToken(TokenValue value, const std::string &name, const std::string &string_value);

    SourceLocation location_;
    std::string input_;
    std::string::size_type index_{};
    std::string buffer_;
    KeywordsDictionary
};

}

#endif //TINY_C_COMPILER_SCANNER_H