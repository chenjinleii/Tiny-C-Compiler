//
// Created by kaiser on 18-12-8.
//

#include "scanner.h"
#include "error.h"

#include <fstream>
#include <cctype>
#include <sstream>
#include <algorithm>

namespace tcc {

Scanner::Scanner(const std::string &file_name) {
    std::ifstream ifs{file_name};
    if (!ifs) {
        ErrorReportAndExit("When trying to open file " + file_name + ", occurred error.");
    }

    std::ostringstream ost;
    std::string line;
    while (std::getline(ifs, line)) {
        ost << line << '\n';
    }

    input_ = ost.str();
    auto iter{std::find_if_not(std::rbegin(input_), std::rend(input_), isspace)};
    input_.erase(iter.base(), std::end(input_));

    keywords_.insert({"auto", {TokenValue::kAutoKeyword, -1}});
    keywords_.insert({"break", {TokenValue::kBreakKeyword, -1}});
    keywords_.insert({"case", {TokenValue::kCaseKeyword, -1}});
    keywords_.insert({"char", {TokenValue::kCharKeyword, -1}});
    keywords_.insert({"const", {TokenValue::kConstKeyword, -1}});
    keywords_.insert({"continue", {TokenValue::kContinueKeyword, -1}});
    keywords_.insert({"default", {TokenValue::kDefaultKeyword, -1}});
    keywords_.insert({"do", {TokenValue::kDoKeyword, -1}});
    keywords_.insert({"double", {TokenValue::kDoubleKeyword, -1}});
    keywords_.insert({"else", {TokenValue::kElseKeyword, -1}});
    keywords_.insert({"enum", {TokenValue::kEnumKeyword, -1}});
    keywords_.insert({"extern", {TokenValue::kExternKeyword, -1}});
    keywords_.insert({"float", {TokenValue::kFloatKeyword, -1}});
    keywords_.insert({"for", {TokenValue::kForKeyword, -1}});
    keywords_.insert({"goto", {TokenValue::kGotoKeyword, -1}});
    keywords_.insert({"if", {TokenValue::kIfKeyword, -1}});
    keywords_.insert({"inline", {TokenValue::kInlineKeyword, -1}});
    keywords_.insert({"int", {TokenValue::kIntKeyword, -1}});
    keywords_.insert({"long", {TokenValue::kLongKeyword, -1}});
    keywords_.insert({"register", {TokenValue::kRegisterKeyword, -1}});
    keywords_.insert({"restrict", {TokenValue::kRestrictKeyword, -1}});
    keywords_.insert({"return", {TokenValue::kReturnKeyword, -1}});
    keywords_.insert({"short", {TokenValue::kShortKeyword, -1}});
    keywords_.insert({"signed", {TokenValue::kSignedKeyword, -1}});
    keywords_.insert({"sizeof", {TokenValue::kSizeofKeyword, -1}});
    keywords_.insert({"static", {TokenValue::kStaticKeyword, -1}});
    keywords_.insert({"struct", {TokenValue::kStructKeyword, -1}});
    keywords_.insert({"switch", {TokenValue::kSwitchKeyword, -1}});
    keywords_.insert({"typedef", {TokenValue::kTypedefKeyword, -1}});
    keywords_.insert({"union", {TokenValue::kUnionKeyword, -1}});
    keywords_.insert({"unsigned", {TokenValue::kUnsignedKeyword, -1}});
    keywords_.insert({"void", {TokenValue::kVoidKeyword, -1}});
    keywords_.insert({"volatile", {TokenValue::kVolatileKeyword, -1}});
    keywords_.insert({"while", {TokenValue::kWhileKeyword, -1}});
    keywords_.insert({"_Bool", {TokenValue::kBoolKeyword, -1}});
    keywords_.insert({"_Complex", {TokenValue::kComplexKeyword, -1}});
    keywords_.insert({"_Imaginary", {TokenValue::kImaginaryKeyword, -1}});
}

std::vector<Token> Scanner::Scan() {
    std::vector<Token> token_sequence;

    while (HasNextChar()) {
        token_sequence.push_back(GetNextToken());
    }

    return token_sequence;
}

Token Scanner::GetNextToken() {
    SkipSpace();

    char current_char{GetNextChar()};
    switch (current_char) {
        case '#': {
            SkipComment();
            return GetNextToken();
        }
        case '(':return MakeToken(TokenValue::kLeftParen, -1, "(");
        case ')':return MakeToken(TokenValue::kRightParen, -1, ")");
        case '[':return MakeToken(TokenValue::kLeftSquare, -1, "[");
        case ']':return MakeToken(TokenValue::kRightSquare, -1, "]");
        case '{':return MakeToken(TokenValue::kLeftCurly, -1, "{");
        case '}':return MakeToken(TokenValue::kRightCurly, -1, "}");
        case '?':return MakeToken(TokenValue::kQuestionMark, -1, "?");
        case ',':return MakeToken(TokenValue::kComma, -1, ",");
        case '~':return MakeToken(TokenValue::kNeg, 0, "~");
        case ';':return MakeToken(TokenValue::kSemicolon, -1, ";");
        case '-': {
            if (Try('>')) {
                return MakeToken(TokenValue::kArrow, 150, "->");
            } else if (Try('-')) {
                return MakeToken(TokenValue::kDec, 150, "--");
            } else if (Try('=')) {
                return MakeToken(TokenValue::kSubAssign, 20, "-=");
            } else {
                return MakeToken(TokenValue::kSub, 120, "-");
            }
        }
        case '+':
            if (Try('+')) {
                return MakeToken(TokenValue::kInc, 150, "++");
            } else if (Try('=')) {
                return MakeToken(TokenValue::kAddAssign, 20, "+=");
            } else {
                return MakeToken(TokenValue::kAdd, 120, "+");
            }

    }
}

Token Scanner::HandleIdentifierOrKeyword() {
    return Token();
}

Token Scanner::HandleNumber() {
    return Token();
}

Token Scanner::HandleChar() {
    return Token();
}

Token Scanner::HandleString() {
    return Token();
}

Token Scanner::HandleEscape() {
    return Token();
}

Token Scanner::HandleOctEscape() {
    return Token();
}

Token Scanner::HandleHexEscape() {
    return Token();
}

void Scanner::SkipSpace() {

}

void Scanner::SkipComment() {

}

char Scanner::GetNextChar() {
    if (HasNextChar()) {
        char next_char{input_[++index_]};
        if (next_char == '\n') {
            ++location_.line_;
            location_.column_ = 0;
        } else {
            ++location_.column_;
        }
        return next_char;
    } else {
        return EOF;
    }
}

char Scanner::PeekNextChar() const {
    if (HasNextChar()) {
        return input_[index_ + 1];
    } else {
        return EOF;
    }
}

bool Scanner::HasNextChar() const {
    return index_ + 1 < std::size(input_);
}

bool Scanner::Try(char ch) {
    if (PeekNextChar() == ch) {
        GetNextChar();
        return true;
    } else {
        return false;
    }
}

bool Scanner::Test(char ch) const {
    return PeekNextChar() == ch;
}

Token Scanner::MakeToken(TokenValue value, const std::string &name) {
    return Token{location_, value, name};
}

Token Scanner::MakeToken(TokenValue value, std::int32_t precedence, const std::string &name) {
    return Token{location_, value, precedence, name};
}

Token Scanner::MakeToken(TokenValue value, const std::string &name, char char_value) {
    return Token{location_, value, name, char_value};
}

Token Scanner::MakeToken(TokenValue value, const std::string &name, std::int32_t int32_value) {
    return Token{location_, value, name, int32_value};
}

Token Scanner::MakeToken(TokenValue value, const std::string &name, double double_value) {
    return Token{location_, value, name, double_value};
}

Token Scanner::MakeToken(TokenValue value, const std::string &name, const std::string &string_value) {
    return Token{location_, value, name, string_value};
}

}

//keywords_.insert("=", {TokenValue::kAssign, 20});
//keywords_.insert("+=", {TokenValue::kAddAssign, 20});
//keywords_.insert("-=", {TokenValue::kSubAssign, 20});
//keywords_.insert("*=", {TokenValue::kMulAssign, 20});
//keywords_.insert("/=", {TokenValue::kDivAssign, 20});
//keywords_.insert("%=", {TokenValue::kModAssign, 20});
//keywords_.insert("&=", {TokenValue::kAndAssign, 20});
//keywords_.insert("|=", {TokenValue::kOrAssign, 20});
//keywords_.insert("^=", {TokenValue::kXorAssign, 20});
//keywords_.insert("<<=", {TokenValue::kShlAssign, 20});
//keywords_.insert(">>=", {TokenValue::kShrAssign, 20});
//
//keywords_.insert("++", {TokenValue::kInc, 150});
//keywords_.insert("--", {TokenValue::kDec, 150});
//
//keywords_.insert("+", {TokenValue::kAdd, 120});
//keywords_.insert("-", {TokenValue::kSub, 120});
//keywords_.insert("*", {TokenValue::kMul, 130});
//keywords_.insert("/", {TokenValue::kDiv, 130});
//keywords_.insert("%", {TokenValue::kMod, 130});
//keywords_.insert("~", {TokenValue::kNeg, 0});
//keywords_.insert("&", {TokenValue::kAnd, 80});
//keywords_.insert("|", {TokenValue::kOr, 60});
//keywords_.insert("^", {TokenValue::kXor, 70});
//keywords_.insert("<<", {TokenValue::kShl, 110});
//keywords_.insert(">>", {TokenValue::kShr, 110});
//
//keywords_.insert("!", {TokenValue::kLogicNeg, 140});
//keywords_.insert("&&", {TokenValue::kLogicAnd, 50});
//keywords_.insert("||", {TokenValue::kLogicOr, 40});
//
//keywords_.insert("==", {TokenValue::kEqual, 90});
//keywords_.insert("!=", {TokenValue::kNotEqual, 90});
//keywords_.insert("<", {TokenValue::kLess, 100});
//keywords_.insert(">", {TokenValue::kGreater, 100});
//keywords_.insert("<=", {TokenValue::kLessOrEqual, 100});
//keywords_.insert(">=", {TokenValue::kGreaterOrEqual, 100});
//
//keywords_.insert("->", {TokenValue::kArrow, 150});
//keywords_.insert(".", {TokenValue::kPeriod, 150});
//
//keywords_.insert(",", {TokenValue::kComma, 10});
//
//keywords_.insert("(", {TokenValue::kLeftParen, -1});
//keywords_.insert(")", {TokenValue::kRightParen, -1});
//keywords_.insert("[", {TokenValue::kLeftSquare, -1});
//keywords_.insert("]", {TokenValue::kRightSquare, -1});
//keywords_.insert("{", {TokenValue::kLeftCurly, -1});
//keywords_.insert("}", {TokenValue::kRightCurly, -1});
//keywords_.insert(";", {TokenValue::kSemicolon, -1});