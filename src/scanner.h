//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_SCANNER_H
#define TINY_C_COMPILER_SCANNER_H

#include "dictionary.h"
#include "token.h"
#include <string>
#include <vector>
#include <fstream>

class Scanner {
public:
    explicit Scanner(const std::string &file_name);
    Token GetNextToken();
    std::vector<Token> GetTokenSequence();
private:
    enum class State {
        kNone,
        kIdentifier,
        kNumber,
        kString,
        kCharacter,
        kOperators
    };

    char GetChar();
    char PeekChar() const;
    void PutBack();
    void Clear();
    void ErrorReport(const std::string &msg);

    void Skip();
    void HandleWell();

    void HandleEscape();
    void HandleChar();
    void HandleString();

    void HandleNumber();
    bool HandleDigit();
    bool HandleFraction();
    void HandleExp();

    void HandleIdentifierOrKeyword();
    void HandleOperatorOrDelimiter();

    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name);
    void MakeToken(TokenType type, TokenValue value,
                   std::int32_t symbol_precedence, const std::string &name);

    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, bool bool_value);
    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, unsigned char unsigned_char_value);
    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, signed char signed_char_value);
    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, char char_value);

    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, short short_value);
    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, int int_value);
    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, long long_value);
    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, long long long_long_value);

    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, unsigned short unsigned_short_value);
    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, unsigned int unsigned_int_value);
    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, unsigned long unsigned_long_value);
    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, unsigned long long unsigned_long_long_value);

    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, float float_value);
    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, double double_value);

    void MakeToken(TokenType type, TokenValue value,
                   const std::string &name, const std::string &string_value);

    char current_char_{};
    std::string input_;
    decltype(input_)::size_type index_{};

    State state_{State::kNone};

    Dictionary dictionary_;
    Token token_;
    std::string buffer_;
};

#endif //TINY_C_COMPILER_SCANNER_H