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
    keywords_.insert({"auto", TokenValue::kAuto});
    keywords_.insert({"break", TokenValue::kBreak});
    keywords_.insert({"case", TokenValue::kCase});
    keywords_.insert({"char", TokenValue::kChar});
    keywords_.insert({"const", TokenValue::kConst});
    keywords_.insert({"continue", TokenValue::kContinue});
    keywords_.insert({"default", TokenValue::kDefault});
    keywords_.insert({"do", TokenValue::kDo});
    keywords_.insert({"double", TokenValue::kDouble});
    keywords_.insert({"else", TokenValue::kElse});
    keywords_.insert({"enum", TokenValue::kEnum});
    keywords_.insert({"extern", TokenValue::kExtern});
    keywords_.insert({"float", TokenValue::kFloat});
    keywords_.insert({"for", TokenValue::kFor});
    keywords_.insert({"goto", TokenValue::kGoto});
    keywords_.insert({"if", TokenValue::kIf});
    keywords_.insert({"inline", TokenValue::kInline});
    keywords_.insert({"int", TokenValue::kInt});
    keywords_.insert({"long", TokenValue::kLong});
    keywords_.insert({"register", TokenValue::kRegister});
    keywords_.insert({"restrict", TokenValue::kRestrict});
    keywords_.insert({"return", TokenValue::kReturn});
    keywords_.insert({"short", TokenValue::kShort});
    keywords_.insert({"signed", TokenValue::kSigned});
    keywords_.insert({"sizeof", TokenValue::kSizeof});
    keywords_.insert({"static", TokenValue::kStatic});
    keywords_.insert({"struct", TokenValue::kStruct});
    keywords_.insert({"switch", TokenValue::kSwitch});
    keywords_.insert({"typedef", TokenValue::kTypedef});
    keywords_.insert({"union", TokenValue::kUnion});
    keywords_.insert({"unsigned", TokenValue::kUnsigned});
    keywords_.insert({"void", TokenValue::kVoid});
    keywords_.insert({"volatile", TokenValue::kVolatile});
    keywords_.insert({"while", TokenValue::kWhile});
    keywords_.insert({"_Bool", TokenValue::kBool});
    keywords_.insert({"_Complex", TokenValue::kComplex});
    keywords_.insert({"_Imaginary", TokenValue::kImaginary});
}

TokenValue KeywordsDictionary::Find(const std::string &name) const {
    if (auto iter{keywords_.find(name)};iter != std::end(keywords_)) {
        return iter->second;
    } else {
        return TokenTypes::kIdentifier;
    }
}

Scanner::Scanner(const std::string &processed_file, const std::string &input_file) {
    std::ifstream ifs{processed_file};
    if (!ifs) {
        ErrorReportAndExit("When trying to open file " + input_file + ", occurred error.");
    }

    std::ostringstream ost;
    std::string line;
    while (std::getline(ifs, line)) {
        ost << line << '\n';
    }

    input_ = ost.str();
    auto iter{std::find_if_not(std::rbegin(input_), std::rend(input_), isspace)};
    input_.erase(iter.base(), std::end(input_));

    auto index{input_file.rfind('/')};
    if (index != std::string::npos) {
        location_.file_name_ = input_file.substr(index + 1);
    } else {
        location_.file_name_ = input_file;
    }
}

std::vector<Token> Scanner::Scan() {
    std::vector<Token> token_sequence;

    while (HasNextChar()) {
        if (auto token{GetNextToken()};!token.TokenValueIs(TokenTypes::kNone) &&
                !token.TokenValueIs(TokenTypes::kEof)) {
            token_sequence.push_back(token);
        }
        buffer_.clear();
    }

    return token_sequence;
}

std::vector<Token> Scanner::Debug(const std::string &processed_file,
                                  const std::string &input_file, std::ostream &os) {
    Scanner scanner{processed_file, input_file};
    auto token_sequence{scanner.Scan()};
    for (const auto &token:token_sequence) {
        os << token.ToString() << '\n';
    }
    std::cout << "token Successfully written\n";

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
        case ':':return MakeToken(TokenValue::KColon);
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
            ErrorReportAndExit(location_, "Unknown character.");
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

    auto token{keywords_dictionary_.Find(buffer_)};
    return MakeToken(token, buffer_);
}

// TODO 类型后缀
Token Scanner::HandleNumber() {
    if (buffer_.front() == '.') {
        auto ch{GetNextChar()};
        while (std::isdigit(ch)) {
            buffer_.push_back(ch);
            ch = GetNextChar();
        }
        PutBack();
        return MakeToken(std::stod(buffer_));
    }

    if (buffer_.front() == '0') {
        auto ch{GetNextChar()};
        if (ch == 'x' || ch == 'X') {
            return MakeToken(HandleHexNumber());
        } else if (IsOctDigit(ch)) {
            buffer_.push_back(ch);
            return MakeToken(HandleOctNumber());
        } else if (ch == '.') {
            do {
                buffer_.push_back(ch);
                ch = GetNextChar();
            } while (std::isdigit(ch));
            PutBack();
            return MakeToken(std::stod(buffer_));
        } else {
            PutBack();
            return MakeToken(0);
        }
    }

    std::int32_t dot_count{};
    auto ch{GetNextChar()};
    while (std::isdigit(ch) || ch == '.') {
        if (ch == '.') {
            ++dot_count;
        }
        buffer_.push_back(ch);
        ch = GetNextChar();
    }

    PutBack();
    if (dot_count > 1) {
        ErrorReportAndExit(location_, "No more than one decimal point");
        return MakeToken(TokenValue::kNone);
    } else if (dot_count == 1) {
        return MakeToken(std::stod(buffer_));
    } else {
        return MakeToken(std::stoi(buffer_));
    }

}

std::int32_t Scanner::HandleOctNumber() {
    auto next{GetNextChar()};
    while (IsOctDigit(next)) {
        buffer_.push_back(next);
        next = GetNextChar();
    }
    PutBack();
    return std::stoi(buffer_, nullptr, 8);
}

std::int32_t Scanner::HandleHexNumber() {
    auto next{GetNextChar()};
    while (std::isxdigit(next)) {
        buffer_.push_back(next);
        next = GetNextChar();
    }
    PutBack();
    return std::stoi(buffer_, nullptr, 16);
}

Token Scanner::HandleChar() {
    char ch{GetNextChar()};

    if (ch == '\\') {
        ch = HandleEscape();
    }
    if (ch == EOF) {
        return MakeToken(TokenTypes::kEof);
    }

    if (auto next{GetNextChar()};next != '\'') {
        ErrorReportAndExit(location_, "miss '");
        return MakeToken(TokenTypes::kNone);
    }

    return MakeToken(ch);
}

//TODO 连接相邻字符串
Token Scanner::HandleString() {
    while (HasNextChar() && !Test('\"') && !Test('\n')) {
        char ch{GetNextChar()};
        if (ch == '\\') {
            ch = HandleEscape();
            if (ch == EOF) {
                return MakeToken(TokenTypes::kNone);
            }
        }
        buffer_.push_back(ch);
    }

    if (GetNextChar() != '\"') {
        ErrorReportAndExit(location_, "miss \"");
        return MakeToken(TokenTypes::kNone);
    }

    return MakeToken(buffer_);
}

char Scanner::HandleEscape() {
    auto ch{GetNextChar()};
    switch (ch) {
        case '\\':
        case '\'':
        case '\"':
        case '\?':return ch;
        case 'a':return '\a';
        case 'b':return '\b';
        case 'f':return '\f';
        case 'n':return '\n';
        case 'r':return '\r';
        case 't':return '\t';
        case 'v':return '\v';
        case 'X':
        case 'x':return HandleHexEscape();
        case '0' ... '7':return HandleOctEscape(ch);
        default: {
            ErrorReportAndExit(location_, "unrecognized escape character '{}'", ch);
            return EOF;
        }
    }
}

char Scanner::HandleOctEscape(char ch) {
    std::string buf;
    buf.push_back(ch);

    for (std::int32_t i{0}; i < 2; ++i) {
        if (char next{GetNextChar()};IsOctDigit(next)) {
            buf.push_back(next);
        } else {
            PutBack();
            break;
        }
    }
    return static_cast<char>(std::stoi(buf, nullptr, 8));
}

char Scanner::HandleHexEscape() {
    std::string buf;

    for (std::int32_t i{0}; i < 2; ++i) {
        if (char next{GetNextChar()};std::isxdigit(next)) {
            buf.push_back(next);
        } else {
            PutBack();
            break;
        }
    }
    return static_cast<char>(std::stoi(buf, nullptr, 16));
}

void Scanner::SkipSpace() {
    while (std::isspace(PeekNextChar())) {
        GetNextChar();
    }
}

void Scanner::SkipComment() {
    std::string buff;
    GetNextChar();
    char ch = GetNextChar();
    while (HasNextChar() && std::isdigit(ch)) {
        buff.push_back(ch);
        ch = GetNextChar();
    }
    PutBack();

    while (HasNextChar() && PeekNextChar() != '\n') {
        GetNextChar();
    }
    GetNextChar();
    location_.row_ = std::stoi(buff);
}

char Scanner::GetNextChar() {
    if (HasNextChar()) {
        char next_char{input_[index_++]};
        if (next_char == '\n') {
            ++location_.row_;
            pre_column_ = location_.column_;
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
        if (location_.column_ > 0) {
            --location_.column_;
        } else {
            --location_.row_;
            location_.column_ = pre_column_;
        }
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

bool IsOctDigit(char ch) {
    return ch >= '0' && ch < '8';
}

}
