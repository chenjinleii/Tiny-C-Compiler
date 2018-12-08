//
// Created by kaiser on 18-12-8.
//

#include "dictionary.h"

Dictionary::Dictionary() {
    AddToken("=", {TokenType::kOperator, TokenValue::kAssign, 20});

    AddToken("++", {TokenType::kOperator, TokenValue::kPlusPlus, 150});
    AddToken("--", {TokenType::kOperator, TokenValue::kMinusMinus, 150});

    AddToken("+", {TokenType::kOperator, TokenValue::kPlus, 120});
    AddToken("-", {TokenType::kOperator, TokenValue::kMinus, 120});
    AddToken("*", {TokenType::kOperator, TokenValue::kMultiply, 130});
    AddToken("/", {TokenType::kOperator, TokenValue::kDivide, 130});
    AddToken("%", {TokenType::kOperator, TokenValue::kMod, 130});
    AddToken("~", {TokenType::kOperator, TokenValue::kNeg, 0});
    AddToken("&", {TokenType::kOperator, TokenValue::kAnd, 80});
    AddToken("|", {TokenType::kOperator, TokenValue::kOr, 60});
    AddToken("^", {TokenType::kOperator, TokenValue::kXor, 70});
    AddToken("<<", {TokenType::kOperator, TokenValue::kShl, 110});
    AddToken(">>", {TokenType::kOperator, TokenValue::kShr, 110});

    AddToken("!", {TokenType::kOperator, TokenValue::kLogicNeg, 140});
    AddToken("&&", {TokenType::kOperator, TokenValue::kLogicAnd, 50});
    AddToken("||", {TokenType::kOperator, TokenValue::kLogicOr, 40});

    AddToken("==", {TokenType::kOperator, TokenValue::kEqual, 90});
    AddToken("!=", {TokenType::kOperator, TokenValue::kNotEqual, 90});
    AddToken("<", {TokenType::kOperator, TokenValue::kLess, 100});
    AddToken(">", {TokenType::kOperator, TokenValue::kGreater, 100});
    AddToken("<=", {TokenType::kOperator, TokenValue::kLessOrEqual, 100});
    AddToken(">=", {TokenType::kOperator, TokenValue::kGreaterOrEqual, 100});

    AddToken("->", {TokenType::kOperator, TokenValue::kArrow, 150});
    AddToken(".", {TokenType::kOperator, TokenValue::kPeriod, 150});

    AddToken(",", {TokenType::kOperator, TokenValue::kComma, 10});

    AddToken("(", {TokenType::kDelimiter, TokenValue::kLeftParen, -1});
    AddToken(")", {TokenType::kDelimiter, TokenValue::kRightParen, -1});
    AddToken("[", {TokenType::kDelimiter, TokenValue::kLeftSquare, -1});
    AddToken("]", {TokenType::kDelimiter, TokenValue::kRightSquare, -1});
    AddToken("{", {TokenType::kDelimiter, TokenValue::kLeftCurly, -1});
    AddToken("}", {TokenType::kDelimiter, TokenValue::kRightCurly, -1});
    AddToken(";", {TokenType::kDelimiter, TokenValue::kSemicolon, -1});

    AddToken("auto", {TokenType::kKeyword, TokenValue::kAutoKey, -1});
    AddToken("break", {TokenType::kKeyword, TokenValue::kBreakKey, -1});
    AddToken("case", {TokenType::kKeyword, TokenValue::kCaseKey, -1});
    AddToken("char", {TokenType::kKeyword, TokenValue::kCharKey, -1});
    AddToken("const", {TokenType::kKeyword, TokenValue::kConstKey, -1});
    AddToken("continue", {TokenType::kKeyword, TokenValue::kContinueKey, -1});
    AddToken("default", {TokenType::kKeyword, TokenValue::kDefaultKey, -1});
    AddToken("do", {TokenType::kKeyword, TokenValue::kDoKey, -1});
    AddToken("double", {TokenType::kKeyword, TokenValue::kDoubleKey, -1});
    AddToken("else", {TokenType::kKeyword, TokenValue::kElseKey, -1});
    AddToken("enum", {TokenType::kKeyword, TokenValue::kEnumKey, -1});
    AddToken("extern", {TokenType::kKeyword, TokenValue::kExternKey, -1});
    AddToken("float", {TokenType::kKeyword, TokenValue::kFloatKey, -1});
    AddToken("for", {TokenType::kKeyword, TokenValue::kForKey, -1});
    AddToken("goto", {TokenType::kKeyword, TokenValue::kGotoKey, -1});
    AddToken("if", {TokenType::kKeyword, TokenValue::kIfKey, -1});
    AddToken("inline", {TokenType::kKeyword, TokenValue::kInlineKey, -1});
    AddToken("int", {TokenType::kKeyword, TokenValue::kIntKey, -1});
    AddToken("long", {TokenType::kKeyword, TokenValue::kLongKey, -1});
    AddToken("register", {TokenType::kKeyword, TokenValue::kRegisterKey, -1});
    AddToken("restrict", {TokenType::kKeyword, TokenValue::kRestrictKey, -1});
    AddToken("return", {TokenType::kKeyword, TokenValue::kReturnKey, -1});
    AddToken("short", {TokenType::kKeyword, TokenValue::kShortKey, -1});
    AddToken("signed", {TokenType::kKeyword, TokenValue::kSignedKey, -1});
    AddToken("sizeof", {TokenType::kKeyword, TokenValue::kSizeofKey, -1});
    AddToken("static", {TokenType::kKeyword, TokenValue::kStaticKey, -1});
    AddToken("struct", {TokenType::kKeyword, TokenValue::kStructKey, -1});
    AddToken("switch", {TokenType::kKeyword, TokenValue::kSwitchKey, -1});
    AddToken("typedef", {TokenType::kKeyword, TokenValue::kTypedefKey, -1});
    AddToken("union", {TokenType::kKeyword, TokenValue::kUnionKey, -1});
    AddToken("unsigned", {TokenType::kKeyword, TokenValue::kUnsignedKey, -1});
    AddToken("void", {TokenType::kKeyword, TokenValue::kVoidKey, -1});
    AddToken("volatile", {TokenType::kKeyword, TokenValue::kVolatileKey, -1});
    AddToken("while", {TokenType::kKeyword, TokenValue::kWhileKey, -1});
    AddToken("_Bool", {TokenType::kKeyword, TokenValue::kBoolKey, -1});
    AddToken("_Complex", {TokenType::kKeyword, TokenValue::kComplexKey, -1});
    AddToken("_Imaginary", {TokenType::kKeyword, TokenValue::kImaginaryKey, -1});
}

std::tuple<TokenType, TokenValue, std::int32_t> Dictionary::LookUp(const std::string &name) const {
    if (auto iter = dictionary_.find(name);iter != dictionary_.end()) {
        return {std::get<0>(iter->second), std::get<1>(iter->second), std::get<2>(iter->second)};
    } else {
        return {TokenType::kIdentifier, TokenValue::kIdentifier, -1};
    }
}

bool Dictionary::HaveToken(const std::string &name) const {
    return dictionary_.find(name) != std::end(dictionary_);
}

void Dictionary::AddToken(const std::string &name, const std::tuple<TokenType, TokenValue, std::int32_t> &token) {
    dictionary_.insert({name, token});
}