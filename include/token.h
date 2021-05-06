#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include <map>
#include <utility>

#include "value.h"

namespace cpplox
{
// When you add a new token, don't forget to update `Scanner::str_to_keyword()`
enum class TokenType
{
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    PREFIX,

    IMPORT,

    cpplox_EOF
};

class Token
{
public:
    Token(TokenType type, std::string lexeme, Value literal, int line, std::string str_line, int column)
        : m_token_type(type)
        , m_lexeme(std::move(lexeme))
        , m_literal(std::move(literal))
        , m_line(line)
        , m_str_line(std::move(str_line))
        , m_column(column)
    {
    }

    [[nodiscard]] inline TokenType token_type() const
    {
        return m_token_type;
    }
    [[nodiscard]] inline std::string lexeme() const
    {
        return m_lexeme;
    }
    [[nodiscard]] inline Value literal() const
    {
        return m_literal;
    }
    [[nodiscard]] inline int line() const
    {
        return m_line;
    }
    [[nodiscard]] inline std::string str_line() const
    {
        return m_str_line;
    }
    [[nodiscard]] inline int column() const
    {
        return m_column;
    }

    friend inline bool operator==(const Token& lhs, const Token& rhs)
    {
        return lhs.m_token_type == rhs.m_token_type && lhs.m_lexeme == rhs.m_lexeme;
    }
    friend inline bool operator==(const Token& lhs, std::string_view rhs)
    {
        return lhs.m_lexeme == rhs;
    }

private:
    TokenType m_token_type;
    std::string m_lexeme;
    Value m_literal;
    int m_line;
    // used to print a line that had an error
    std::string m_str_line;
    int m_column;
};
}

#endif  // TOKEN_TYPE_H
