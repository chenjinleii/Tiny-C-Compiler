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

KeywordsDictionary::KeywordsDictionary() {
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

std::pair<TokenValue, std::int32_t> KeywordsDictionary::Find(const std::string &name) {
    if (auto iter{keywords_.find(name)};iter != std::end(keywords_)) {
        return {iter->second.first, iter->second.second};
    } else {
        return {TokenValues::kIdentifier, -1};
    }
}

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
}

std::vector<Token> Scanner::Scan() {
    std::vector<Token> token_sequence;

    while (HasNextChar()) {
        if (auto token{GetNextToken()};token.GetTokenValue() != TokenValue::kNone &&
                token.GetTokenValue() != TokenValue::kEof) {
            token_sequence.push_back(token);
        }
        buffer_.clear();
    }

    return token_sequence;
}

Token Scanner::GetNextToken() {
    SkipSpace();

    char ch{GetNextChar()};
    switch (ch) {
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
        case '+': {
            if (Try('+')) {
                return MakeToken(TokenValue::kInc, 150, "++");
            } else if (Try('=')) {
                return MakeToken(TokenValue::kAddAssign, 20, "+=");
            } else {
                return MakeToken(TokenValue::kAdd, 120, "+");
            }
        }
        case '<': {
            if (Try('<')) {
                if (Try('=')) {
                    return MakeToken(TokenValue::kShlAssign, 20, "<<=");
                } else {
                    return MakeToken(TokenValue::kShl, 110, "<<");
                }
            } else if (Try('=')) {
                return MakeToken(TokenValue::kLessOrEqual, 100, "<=");
            } else {
                return MakeToken(TokenValue::kLess, 100, "<");
            }
        }
        case '>': {
            if (Try('>')) {
                if (Try('=')) {
                    return MakeToken(TokenValue::kShrAssign, 20, ">>=");
                } else {
                    return MakeToken(TokenValue::kShr, 110, ">>");
                }
            } else if (Try('=')) {
                return MakeToken(TokenValue::kGreaterOrEqual, 100, ">=");
            } else {
                return MakeToken(TokenValue::kGreater, 100, ">");
            }
        }
        case '%': {
            if (Try('=')) {
                return MakeToken(TokenValue::kModAssign, 20, "%=");
            } else {
                return MakeToken(TokenValue::kMod, 130, "%");
            }
        }
        case '=': {
            if (Try('=')) {
                return MakeToken(TokenValue::kEqual, 90, "==");
            } else {
                return MakeToken(TokenValue::kAssign, 20, "=");
            }
        }
        case '!': {
            if (Try('=')) {
                return MakeToken(TokenValue::kNotEqual, 90, "!=");
            } else {
                return MakeToken(TokenValue::kLogicNeg, 140, "!");
            }
        }
        case '&': {
            if (Try('&')) {
                return MakeToken(TokenValue::kLogicAnd, 50, "&&");
            } else if (Try('=')) {
                return MakeToken(TokenValue::kAndAssign, 20, "&=");
            } else {
                return MakeToken(TokenValue::kAnd, 80, "&");
            }
        }
        case '|': {
            if (Try('|')) {
                return MakeToken(TokenValue::kLogicOr, 40, "||");
            } else if (Try('=')) {
                return MakeToken(TokenValue::kOrAssign, 20, "|=");
            } else {
                return MakeToken(TokenValue::kOr, 60, "|");
            }
        }
        case '*': {
            if (Try('=')) {
                return MakeToken(TokenValue::kMulAssign, 20, "*=");
            } else {
                return MakeToken(TokenValue::kMul, 130, "*");
            }
        }
        case '/': {
            if (Try('=')) {
                return MakeToken(TokenValue::kDivAssign, 20, "/=");
            } else {
                return MakeToken(TokenValue::kDiv, 130, "/");
            }
        }
        case '^': {
            if (Try('=')) {
                return MakeToken(TokenValue::kXorAssign, 20, "^=");
            } else {
                return MakeToken(TokenValue::kXor, 70, "^");
            }
        }
        case '.': {
            if (std::isdigit(PeekNextChar())) {
                buffer_.push_back(ch);
                return HandleNumber();
            } else if (auto[next, next_two]{PeekNextTwoChar()};next == '.' && next_two == '.') {
                return MakeToken(TokenValue::kEllipsis, -1, "...");
            } else {
                return MakeToken(TokenValue::kPeriod, 150, ".");
            }
        }
        case '0'...'9': {
            buffer_.push_back(ch);
            return HandleNumber();
        }
        case '\'':return HandleChar();
        case '\"':return HandleString();
        case 'a'...'z':
        case 'A'...'Z':
        case '_': {
            buffer_.push_back(ch);
            return HandleIdentifierOrKeyword();
        }
        case EOF:return MakeToken(TokenValue::kEof, -1, "eof");
        default: {
            ErrorReport(location_, "Unknown character.");
            return MakeToken(TokenValue::kNone, -1, "none");
        }
    }
}

Token Scanner::HandleIdentifierOrKeyword() {
    char ch{GetNextChar()};
    while (std::isalnum(ch) || ch == '_') {
        buffer_.push_back(ch);
        ch = GetNextChar();
    }
    PutBack();

    auto token{keywords_.Find(buffer_)};
    return MakeToken(token.first, token.second, buffer_);
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
        char next_char{input_[index_++]};
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
        return input_[index_];
    } else {
        return EOF;
    }
}

std::pair<char, char> Scanner::PeekNextTwoChar() const {
    std::pair<char, char> ret{EOF, EOF};
    if (index_ + 1 < std::size(input_)) {
        ret = {input_[index_], input_[index_ + 1]};
    }
    return ret;
}

void Scanner::PutBack() {
    if (index_ > 0) {
        --index_;
    }
}

bool Scanner::HasNextChar() const {
    return index_ < std::size(input_);
}

bool Scanner::Try(char ch) {
    if (PeekNextChar() == ch) {
        GetNextChar();
        return true;
    } else {
        return false;
    }
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