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
    keywords_.insert({"auto", TokenValue::kAutoKeyword});
    keywords_.insert({"break", TokenValue::kBreakKeyword});
    keywords_.insert({"case", TokenValue::kCaseKeyword});
    keywords_.insert({"char", TokenValue::kCharKeyword});
    keywords_.insert({"const", TokenValue::kConstKeyword});
    keywords_.insert({"continue", TokenValue::kContinueKeyword});
    keywords_.insert({"default", TokenValue::kDefaultKeyword});
    keywords_.insert({"do", TokenValue::kDoKeyword});
    keywords_.insert({"double", TokenValue::kDoubleKeyword});
    keywords_.insert({"else", TokenValue::kElseKeyword});
    keywords_.insert({"enum", TokenValue::kEnumKeyword});
    keywords_.insert({"extern", TokenValue::kExternKeyword});
    keywords_.insert({"float", TokenValue::kFloatKeyword});
    keywords_.insert({"for", TokenValue::kForKeyword});
    keywords_.insert({"goto", TokenValue::kGotoKeyword});
    keywords_.insert({"if", TokenValue::kIfKeyword});
    keywords_.insert({"inline", TokenValue::kInlineKeyword});
    keywords_.insert({"int", TokenValue::kIntKeyword});
    keywords_.insert({"long", TokenValue::kLongKeyword});
    keywords_.insert({"register", TokenValue::kRegisterKeyword});
    keywords_.insert({"restrict", TokenValue::kRestrictKeyword});
    keywords_.insert({"return", TokenValue::kReturnKeyword});
    keywords_.insert({"short", TokenValue::kShortKeyword});
    keywords_.insert({"signed", TokenValue::kSignedKeyword});
    keywords_.insert({"sizeof", TokenValue::kSizeofKeyword});
    keywords_.insert({"static", TokenValue::kStaticKeyword});
    keywords_.insert({"struct", TokenValue::kStructKeyword});
    keywords_.insert({"switch", TokenValue::kSwitchKeyword});
    keywords_.insert({"typedef", TokenValue::kTypedefKeyword});
    keywords_.insert({"union", TokenValue::kUnionKeyword});
    keywords_.insert({"unsigned", TokenValue::kUnsignedKeyword});
    keywords_.insert({"void", TokenValue::kVoidKeyword});
    keywords_.insert({"volatile", TokenValue::kVolatileKeyword});
    keywords_.insert({"while", TokenValue::kWhileKeyword});
    keywords_.insert({"_Bool", TokenValue::kBoolKeyword});
    keywords_.insert({"_Complex", TokenValue::kComplexKeyword});
    keywords_.insert({"_Imaginary", TokenValue::kImaginaryKeyword});
}

TokenValue KeywordsDictionary::Find(const std::string &name) {
    if (auto iter{keywords_.find(name)};iter != std::end(keywords_)) {
        return iter->second;
    } else {
        return TokenValues::kIdentifier;
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
        case '(':return MakeToken(TokenValue::kLeftParen);
        case ')':return MakeToken(TokenValue::kRightParen);
        case '[':return MakeToken(TokenValue::kLeftSquare);
        case ']':return MakeToken(TokenValue::kRightSquare);
        case '{':return MakeToken(TokenValue::kLeftCurly);
        case '}':return MakeToken(TokenValue::kRightCurly);
        case '?':return MakeToken(TokenValue::kQuestionMark);
        case ',':return MakeToken(TokenValue::kComma);
        case '~':return MakeToken(TokenValue::kNeg);
        case ';':return MakeToken(TokenValue::kSemicolon);
        case '-': {
            if (Try('>')) {
                return MakeToken(TokenValue::kArrow);
            } else if (Try('-')) {
                return MakeToken(TokenValue::kDec);
            } else if (Try('=')) {
                return MakeToken(TokenValue::kSubAssign);
            } else {
                return MakeToken(TokenValue::kSub);
            }
        }
        case '+': {
            if (Try('+')) {
                return MakeToken(TokenValue::kInc);
            } else if (Try('=')) {
                return MakeToken(TokenValue::kAddAssign);
            } else {
                return MakeToken(TokenValue::kAdd);
            }
        }
        case '<': {
            if (Try('<')) {
                if (Try('=')) {
                    return MakeToken(TokenValue::kShlAssign);
                } else {
                    return MakeToken(TokenValue::kShl);
                }
            } else if (Try('=')) {
                return MakeToken(TokenValue::kLessOrEqual);
            } else {
                return MakeToken(TokenValue::kLess);
            }
        }
        case '>': {
            if (Try('>')) {
                if (Try('=')) {
                    return MakeToken(TokenValue::kShrAssign);
                } else {
                    return MakeToken(TokenValue::kShr);
                }
            } else if (Try('=')) {
                return MakeToken(TokenValue::kGreaterOrEqual);
            } else {
                return MakeToken(TokenValue::kGreater);
            }
        }
        case '%': {
            if (Try('=')) {
                return MakeToken(TokenValue::kModAssign);
            } else {
                return MakeToken(TokenValue::kMod);
            }
        }
        case '=': {
            if (Try('=')) {
                return MakeToken(TokenValue::kEqual);
            } else {
                return MakeToken(TokenValue::kAssign);
            }
        }
        case '!': {
            if (Try('=')) {
                return MakeToken(TokenValue::kNotEqual);
            } else {
                return MakeToken(TokenValue::kLogicNeg);
            }
        }
        case '&': {
            if (Try('&')) {
                return MakeToken(TokenValue::kLogicAnd);
            } else if (Try('=')) {
                return MakeToken(TokenValue::kAndAssign);
            } else {
                return MakeToken(TokenValue::kAnd);
            }
        }
        case '|': {
            if (Try('|')) {
                return MakeToken(TokenValue::kLogicOr);
            } else if (Try('=')) {
                return MakeToken(TokenValue::kOrAssign);
            } else {
                return MakeToken(TokenValue::kOr);
            }
        }
        case '*': {
            if (Try('=')) {
                return MakeToken(TokenValue::kMulAssign);
            } else {
                return MakeToken(TokenValue::kMul);
            }
        }
        case '/': {
            if (Try('=')) {
                return MakeToken(TokenValue::kDivAssign);
            } else {
                return MakeToken(TokenValue::kDiv);
            }
        }
        case '^': {
            if (Try('=')) {
                return MakeToken(TokenValue::kXorAssign);
            } else {
                return MakeToken(TokenValue::kXor);
            }
        }
        case '.': {
            if (std::isdigit(PeekNextChar())) {
                buffer_.push_back(ch);
                return HandleNumber();
            } else if (auto[next, next_two]{PeekNextTwoChar()};next == '.' && next_two == '.') {
                return MakeToken(TokenValue::kEllipsis);
            } else {
                return MakeToken(TokenValue::kPeriod);
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
        case EOF:return MakeToken(TokenValue::kEof);
        default: {
            ErrorReport(location_, "Unknown character.");
            return MakeToken(TokenValue::kNone);
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
    return MakeToken(token, buffer_);
}

Token Scanner::HandleNumber() {
    return Token();
}

Token Scanner::HandleChar() {
    char ch{GetNextChar()};

    if (ch == '\\') {
        ch = HandleEscape();
    }

    if (GetNextChar() != '\'') {
        return MakeToken(TokenValues::kNone);
    }

    return MakeToken(ch);
}

Token Scanner::HandleString() {
    return Token();
}

char Scanner::HandleEscape() {

}

char Scanner::HandleOctEscape() {

}

char Scanner::HandleHexEscape() {

}

void Scanner::SkipSpace() {
    while (std::isspace(PeekNextChar())) {
        GetNextChar();
    }
}

void Scanner::SkipComment() {
    while (HasNextChar() && PeekNextChar() != '\n') {
        GetNextChar();
    }
}

char Scanner::GetNextChar() {
    if (HasNextChar()) {
        char next_char{input_[index_++]};
        if (next_char == '\n') {
            ++location_.row_;
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

bool Scanner::Test(char ch) const {
    return PeekNextChar() == ch;
}

Token Scanner::MakeToken(TokenValue value) {
    return Token{location_, value};
}

Token Scanner::MakeToken(TokenValue value, const std::string &name) {
    return Token{location_, value, name};
}

Token Scanner::MakeToken(char char_value) {
    return Token{location_, char_value};
}

Token Scanner::MakeToken(std::int32_t int32_value) {
    return Token{location_, int32_value};
}

Token Scanner::MakeToken(double double_value) {
    return Token{location_, double_value};
}

Token Scanner::MakeToken(const std::string &string_value) {
    return Token{location_, string_value};
}

}