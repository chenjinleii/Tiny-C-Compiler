//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_DICTIONARY_H
#define TINY_C_COMPILER_DICTIONARY_H

#include "token.h"
#include <string>
#include <cstdint>
#include <unordered_map>

class Dictionary {
public:
    Dictionary();
    std::tuple<TokenType, TokenValue, std::int32_t> LookUp(const std::string &name) const;
    bool HaveToken(const std::string &name) const;
private:
    void AddToken(const std::string &name, const std::tuple<TokenType, TokenValue, std::int32_t> &token);
    std::unordered_map<std::string, std::tuple<TokenType, TokenValue, std::int32_t>> dictionary_;
};

#endif //TINY_C_COMPILER_DICTIONARY_H