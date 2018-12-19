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
#include <iostream>

namespace tcc {

class KeywordsDictionary {
 public:
  KeywordsDictionary();
  TokenValue Find(const std::string &name) const;
 private:
  std::unordered_map<std::string, TokenValue> keywords_;
};

class Scanner {
 public:
  explicit Scanner(const std::string &processed_file, const std::string &input_file);
  std::vector<Token> Scan();
  static std::vector<Token> Test(const std::string &processed_file,
                                 const std::string &input_file,
                                 std::ostream &os = std::cout);
 private:
  Token GetNextToken();

  Token HandleIdentifierOrKeyword();
  Token HandleNumber();
  std::int32_t HandleOctNumber();
  std::int32_t HandleHexNumber();
  Token HandleChar();
  Token HandleString();
  char HandleEscape();
  char HandleOctEscape(char ch);
  char HandleHexEscape();

  void SkipSpace();
  void SkipComment();

  char GetNextChar();
  char PeekNextChar() const;
  std::pair<char, char> PeekNextTwoChar() const;
  void PutBack();
  bool HasNextChar() const;
  bool Try(char ch);
  bool Test(char ch) const;

  Token MakeToken(TokenValue value);
  Token MakeToken(TokenValue value, const std::string &name);
  Token MakeToken(char char_value);
  Token MakeToken(std::int32_t int32_value);
  Token MakeToken(double double_value);
  Token MakeToken(const std::string &string_value);

  SourceLocation location_;
  std::int32_t pre_column_{};
  std::string input_;
  std::string::size_type index_{};
  std::string buffer_;
  KeywordsDictionary keywords_dictionary_;
};

bool IsOctDigit(char ch);

}

#endif //TINY_C_COMPILER_SCANNER_H
