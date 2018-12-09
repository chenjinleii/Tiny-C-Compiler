//
// Created by kaiser on 18-12-8.
//

//TODO 各种类型后缀
#include "scanner.h"
#include <fstream>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <cctype>
#include <filesystem>
#include <sstream>

Scanner::Scanner(const std::string &file_name) {
    std::ifstream ifs{file_name};
    if (!ifs) {
        ErrorReport("When trying to open file " + file_name + ", occurred error.");
    }

    std::ostringstream ost;
    std::string line;
    while (std::getline(ifs, line)) {
        ost << line << '\n';
    }

    input_ = ost.str();
}

std::vector<Token> Scanner::GetTokenSequence() {
    std::vector<Token> ret;

    GetNextToken();
    while (token_.GetTokenType() != TokenType::kEof) {
        ret.push_back(token_);
        GetNextToken();
    }
    return ret;
}

char Scanner::GetChar() {
    if (index_ == std::size(input_)) {
        current_char_ = EOF;
    } else {
        current_char_ = input_[index_++];
    }

    return current_char_;
}

char Scanner::PeekChar() const {
    if (index_ == std::size(input_)) {
        return EOF;
    } else {
        return input_[index_];
    }
}

void Scanner::PutBack() {
    --index_;
    current_char_ = input_[index_ - 1];
}

void Scanner::Clear() {
    buffer_.clear();
    state_ = State::kNone;
}

Token Scanner::GetNextToken() {
    bool matched = false;

    try {
        do {
            if (state_ != State::kNone) {
                matched = true;
            }

            switch (state_) {
                case State::kNone:GetChar();
                    break;

                case State::kIdentifier:HandleIdentifierOrKeyword();
                    break;

                case State::kNumber:HandleNumber();
                    break;

                case State::kString:HandleString();
                    break;

                case State::kCharacter:HandleChar();
                    break;

                case State::kOperators:HandleOperatorOrDelimiter();
                    break;

                default:ErrorReport("Match token state error.");
            }

            if (state_ == State::kNone) {
                Skip();

                if (current_char_ == EOF) {
                    MakeToken(TokenType::kEof, TokenValue::kUnreserved, "end of file", -1);
                    Clear();
                    return token_;
                } else {
                    if (std::isalpha(current_char_) || current_char_ == '_') {
                        state_ = State::kIdentifier;
                    } else if (std::isdigit(current_char_) || (current_char_ == '.' && std::isdigit(PeekChar()))) {
                        state_ = State::kNumber;
                    } else if (current_char_ == '\"') {
                        state_ = State::kString;
                    } else if (current_char_ == '\'') {
                        state_ = State::kCharacter;
                    } else {
                        state_ = State::kOperators;
                    }
                }
            }
        } while (!matched);
    } catch (const std::out_of_range &err) {
        MakeToken(TokenType::kEof, TokenValue::kUnreserved, "end of file", -1);
        Clear();
        return token_;
    }

    Clear();
    return token_;
}

void Scanner::ErrorReport(const std::string &msg) {
    std::cerr << "Token error: " << msg << '\n';
    exit(EXIT_FAILURE);
}

void Scanner::Skip() {
    do {
        while (std::isspace(current_char_)) {
            GetChar();
        }
        HandleWell();
    } while (std::isspace(current_char_));
}

void Scanner::HandleWell() {
    if (current_char_ == '#') {
        while (current_char_ != '\n') {
            if (GetChar() == EOF) {
                throw std::out_of_range("eof");
            }
        }
    }
}

void Scanner::HandleEscape() {
    std::string buffer;

    GetChar();
    switch (current_char_) {
        case '\'':current_char_ = '\'';
            break;
        case '\"':current_char_ = '\"';
            break;
        case '\?':current_char_ = '\?';
            break;
        case '\\':current_char_ = '\\';
            break;
        case 'a':current_char_ = '\a';
            break;
        case 'b':current_char_ = '\b';
            break;
        case 'f':current_char_ = '\f';
            break;
        case 'n':current_char_ = '\n';
            break;
        case 'r':current_char_ = '\r';
            break;
        case 't':current_char_ = '\t';
            break;
        case 'v':current_char_ = '\v';
            break;
        default:
            if (current_char_ == 'x' || current_char_ == 'X') {
                GetChar();
                std::string num;
                while (std::isdigit(current_char_)) {
                    num.push_back(current_char_);
                    if (std::size(num) == 2) {
                        break;
                    }
                    GetChar();
                }

                if (std::size(num) == 0) {
                    ErrorReport("miss number");
                }

                current_char_ = static_cast<char>(std::stoi(num, nullptr, 16));
            } else if (std::isdigit(current_char_)) {
                std::string num;
                do {
                    num.push_back(current_char_);
                    if (std::size(num) == 3) {
                        break;
                    }
                    GetChar();
                } while (std::isdigit(current_char_));

                if (std::size(num) == 0) {
                    ErrorReport("miss number");
                }

                current_char_ = static_cast<char>(std::stoi(num, nullptr, 8));
            }
            break;
    }
}

void Scanner::HandleString() {
    GetChar();
    bool ok{false};
    bool escape_quotes{false};
    bool new_line{false};

    while (!new_line) {
        if (current_char_ == EOF) {
            throw std::out_of_range("eof");
        } else if (current_char_ == '\"' && !escape_quotes) {
            ok = true;
            break;
        }
        buffer_.push_back(current_char_);
        GetChar();
        escape_quotes = false;
        if (current_char_ == '\n') {
            new_line = true;
        } else if (current_char_ == '\\') {
            HandleEscape();
            if (current_char_ == '\"') {
                escape_quotes = true;
            }
            if (current_char_ == '\n') {
                new_line = false;
            }
        }
    }

    if (!ok) {
        ErrorReport("string error");
    }

    while (std::isspace(PeekChar())) {
        for (auto i{index_ + 1};; ++i) {
            if (i >= std::size(input_)) {
                MakeToken(TokenType::kString, TokenValue::kUnreserved, buffer_, buffer_);
                return;
            }
            if (std::isspace(input_[i])) {
                continue;
            } else if (input_[i] == '\"') {
                index_ = i;
                GetChar();

                GetChar();
                bool ok2{false};

                while (current_char_ != '\n') {
                    if (current_char_ == EOF) {
                        throw std::out_of_range("eof");
                    } else if (current_char_ == '\"') {
                        ok2 = true;
                        break;
                    }
                    buffer_.push_back(current_char_);
                    GetChar();
                }

                if (!ok2) {
                    ErrorReport("string error");
                }
                break;
            }
        }
    }
    MakeToken(TokenType::kString, TokenValue::kUnreserved, buffer_, buffer_);
}

void Scanner::HandleChar() {
    GetChar();

    if (current_char_ == '\\') {
        HandleEscape();
    }

    buffer_.push_back(current_char_);
    GetChar();
    if (current_char_ != '\'') {
        ErrorReport("miss \'");
    } else {
        MakeToken(TokenType::kCharacter, TokenValue::kUnreserved, buffer_, buffer_[0]);
    }
}

void Scanner::HandleNumber() {
    if (current_char_ == '.') {
        buffer_.push_back(current_char_);
        GetChar();
        while (std::isdigit(current_char_)) {
            buffer_.push_back(current_char_);
            GetChar();
        }
        PutBack();
        MakeToken(TokenType::kDouble, TokenValue::kUnreserved, buffer_, std::stod(buffer_));
        return;
    }

    bool is_float{false};
    bool is_exp{false};
    bool is_long{false};
    bool is_sin{false};

    enum class NumberState {
        kInteger,
        kFraction,
        kExp,
        kDone
    };

    NumberState number_state{NumberState::kInteger};

    std::int32_t base{10};

    if (current_char_ == '0') {
        base = 8;
    }

    buffer_.push_back(current_char_);
    GetChar();

    if (!std::isdigit(current_char_)) {
        PutBack();
    }

    if (current_char_ == 'x' || current_char_ == 'X') {
        if (std::size(buffer_) == 1 && buffer_.front() == '0') {
            base = 16;
            buffer_.push_back(current_char_);
            GetChar();
            if (!std::isdigit(current_char_)) {
                ErrorReport("number error");
            }
        } else {
            ErrorReport("number error");
        }
    }

    do {
        switch (number_state) {
            case NumberState::kInteger:is_long = HandleDigit();
                break;

            case NumberState::kFraction:is_sin = HandleFraction();
                is_float = true;
                break;
            case NumberState::kExp:HandleExp();
                is_exp = true;
                break;
            case NumberState::kDone:break;
        }
        if (is_long || is_sin) {
            break;
        }
        if (current_char_ == '.') {
            if (is_float) {
                ErrorReport("Fraction number can not have more than one dot.");
            }
            number_state = NumberState::kFraction;
        } else if (current_char_ == 'e' || current_char_ == 'E') {
            if (is_exp) {
                ErrorReport("number error");
            }
            number_state = NumberState::kExp;
        } else if (std::isalpha(current_char_)) {
            ErrorReport("number error");
        } else {
            number_state = NumberState::kDone;
        }
    } while (number_state != NumberState::kDone);

    if (is_long) {
        MakeToken(TokenType::kLongInterger, TokenValue::kUnreserved, buffer_,
                  std::stol(buffer_, nullptr, base));
    } else if (is_sin) {
        MakeToken(TokenType::kFolat, TokenValue::kUnreserved, buffer_,
                  std::stof(buffer_));
    } else if (is_float || is_exp) {
        MakeToken(TokenType::kDouble, TokenValue::kUnreserved, buffer_,
                  std::stod(buffer_));
    } else {
        MakeToken(TokenType::kInterger, TokenValue::kUnreserved, buffer_,
                  std::stoi(buffer_, nullptr, base));
    }
}

bool Scanner::HandleDigit() {
    if (!std::isdigit(current_char_)) {
        return false;
    }

    buffer_.push_back(current_char_);
    GetChar();

    while (std::isdigit(current_char_)) {
        buffer_.push_back(current_char_);
        GetChar();
    }
    PutBack();

    if (current_char_ == 'L' || current_char_ == 'l') {
        GetChar();
        return true;
    }
    return false;
}

bool Scanner::HandleFraction() {
    buffer_.push_back(current_char_);

    if (!std::isdigit(PeekChar())) {
        GetChar();
        buffer_.push_back('0');
        return false;
    }

    GetChar();
    while (std::isdigit(current_char_)) {
        buffer_.push_back(current_char_);
        GetChar();
    }
    PutBack();

    if (current_char_ == 'f' || current_char_ == 'F') {
        GetChar();
        return true;
    }
    return false;
}

void Scanner::HandleExp() {
    buffer_.push_back(current_char_);

    if (!std::isdigit(PeekChar())) {
        if (PeekChar() == '+' || PeekChar() == '-') {
            GetChar();
            buffer_.push_back(current_char_);
        } else {
            ErrorReport("number error");
        }
    }

    GetChar();
    while (std::isdigit(current_char_)) {
        buffer_.push_back(current_char_);
        GetChar();
    }
}

void Scanner::HandleIdentifierOrKeyword() {
    buffer_.push_back(current_char_);
    GetChar();

    while (std::isalnum(current_char_) || current_char_ == '_') {
        buffer_.push_back(current_char_);
        GetChar();
    }
    PutBack();

    auto token{dictionary_.LookUp(buffer_)};
    MakeToken(std::get<0>(token), std::get<1>(token), std::get<2>(token), buffer_);
}

void Scanner::HandleOperatorOrDelimiter() {
    buffer_.push_back(current_char_);
    buffer_.push_back(PeekChar());

    if (dictionary_.HaveToken(buffer_)) {
        GetChar();
    } else {
        buffer_.pop_back();
    }

    auto token{dictionary_.LookUp(buffer_)};
    MakeToken(std::get<0>(token), std::get<1>(token), std::get<2>(token), buffer_);
}

void Scanner::MakeToken(TokenType type, TokenValue value, const std::string &name) {
    token_ = Token{type, value, name};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        std::int32_t symbol_precedence,
                        const std::string &name) {
    token_ = Token{type, value, symbol_precedence, name};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        bool bool_value) {
    token_ = Token{type, value, name, bool_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        unsigned char unsigned_char_value) {
    token_ = Token{type, value, name, unsigned_char_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        signed char signed_char_value) {
    token_ = Token{type, value, name, signed_char_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        char char_value) {
    token_ = Token{type, value, name, char_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        short short_value) {
    token_ = Token{type, value, name, short_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        int int_value) {
    token_ = Token{type, value, name, int_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        long long_value) {
    token_ = Token{type, value, name, long_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        long long long_long_value) {
    token_ = Token{type, value, name, long_long_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        unsigned short unsigned_short_value) {
    token_ = Token{type, value, name, unsigned_short_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        unsigned int unsigned_int_value) {
    token_ = Token{type, value, name, unsigned_int_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        unsigned long unsigned_long_value) {
    token_ = Token{type, value, name, unsigned_long_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        unsigned long long unsigned_long_long_value) {
    token_ = Token{type, value, name, unsigned_long_long_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        float float_value) {
    token_ = Token{type, value, name, float_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        double double_value) {
    token_ = Token{type, value, name, double_value};
}

void Scanner::MakeToken(TokenType type,
                        TokenValue value,
                        const std::string &name,
                        const std::string &string_value) {
    token_ = Token{type, value, name, string_value};
}