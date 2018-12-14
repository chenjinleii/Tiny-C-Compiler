//
// Created by kaiser on 18-12-8.
//

#ifndef TINY_C_COMPILER_TOKEN_H
#define TINY_C_COMPILER_TOKEN_H

#include "location.h"

#include <QObject>

#include <string>
#include <cstdint>
#include <unordered_map>

namespace tcc {

class TokenTypes : public QObject {
Q_OBJECT
public:
    enum Types {
        kCharConstant,
        kIntConstant,
        kDoubleConstant,
        kStringLiteral,

        kIdentifier,

        kAuto,
        kBreak,
        kCase,
        kChar,
        kConst,
        kContinue,
        kDefault,
        kDo,
        kDouble,
        kElse,
        kEnum,
        kExtern,
        kFloat,
        kFor,
        kGoto,
        kIf,
        kInline,
        kInt,
        kLong,
        kRegister,
        kRestrict,
        kReturn,
        kShort,
        kSigned,
        kSizeof,
        kStatic,
        kStruct,
        kSwitch,
        kTypedef,
        kUnion,
        kUnsigned,
        kVoid,
        kVolatile,
        kWhile,
        kBool,
        kComplex,
        kImaginary,

        kAssign,           // =
        kAddAssign,        // +=
        kSubAssign,        // -=
        kMulAssign,        // *=
        kDivAssign,        // /=
        kModAssign,        // %=
        kAndAssign,        // &=
        kOrAssign,         // |=
        kXorAssign,        // ^=
        kShlAssign,        // <<=
        kShrAssign,        // >>=

        kInc,              // ++
        kDec,              // --

        kAdd,              // +
        kSub,              // -
        kMul,              // *
        kDiv,              // /
        kMod,              // %
        kNeg,              // ~
        kAnd,              // &
        kOr,               // |
        kXor,              // ^
        kShl,              // <<
        kShr,              // >>

        kLogicNeg,         // !
        kLogicAnd,         // &&
        kLogicOr,          // ||

        kEqual,            // ==
        kNotEqual,         // !=
        kLess,             // <
        kGreater,          // >
        kLessOrEqual,      // <=
        kGreaterOrEqual,   // >=

        kArrow,            // ->
        kPeriod,           // .

        kQuestionMark,     // ?
        kComma,            // ,

        kLeftParen,        // (
        kRightParen,       // )
        kLeftSquare,       // [
        kRightSquare,      // ]
        kLeftCurly,        // {
        kRightCurly,       // }
        kSemicolon,        // ;
        KColon,            // :
        kEllipsis,         // ...

        kNone,
        kEof
    };
    Q_ENUM(Types)

    static std::string ToString(Types Type);
};

using TokenValue=TokenTypes::Types;

class PrecedenceDictionary {
public:
    PrecedenceDictionary();
    std::int32_t Find(TokenValue value) const;
private:
    std::unordered_map<TokenValue, std::int32_t> precedence_;
};

class Token {
public:
    Token() = default;
    Token(const SourceLocation &location, TokenValue value);
    Token(const SourceLocation &location, TokenValue value, const std::string &name);
    Token(const SourceLocation &location, char char_value);
    Token(const SourceLocation &location, std::int32_t int32_value);
    Token(const SourceLocation &location, double double_value);
    Token(const SourceLocation &location, const std::string &string_value);

    std::string ToString() const;

    bool IsChar() const;
    bool IsInt32() const;
    bool IsDouble() const;
    bool IsString() const;

    bool IsTypeSpecifier() const;
    bool IsIdentifier() const;

    bool TokenValueIs(TokenValue value) const;
    SourceLocation GetTokenLocation() const;
    TokenValue GetTokenValue() const;
    std::string GetTokenName() const;
    std::int32_t GetTokenPrecedence() const;

    char GetCharValue() const;
    std::int32_t GetInt32Value() const;
    double GetDoubleValue() const;
    std::string GetStringValue() const;
    void AppendStringValue(const std::string &str);
private:
    SourceLocation location_;
    TokenValue value_{TokenValue::kNone};
    std::string name_;
    std::int32_t precedence_{-1};

    char char_value_{};
    std::int32_t int32_value_{};
    double double_value_{};
    std::string string_value_;
    PrecedenceDictionary precedence_dictionary_;
};

}

#endif //TINY_C_COMPILER_TOKEN_H
