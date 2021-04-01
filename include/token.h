#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include <map>
#include <utility>

#include "value.h"

namespace cpplox
{
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

    cpplox_EOF
};

class Token
{
public:
    Token(TokenType type, std::string lexeme, Value literal, unsigned int line, const std::string& str_line,
          unsigned int column)
        : m_token_type(type)
        , m_lexeme(std::move(lexeme))
        , m_literal(std::move(literal))
        , m_line(line)
        , m_str_line(str_line)
        , m_column(column)
    {
    }

    [[nodiscard]] inline TokenType get_token_type() const
    {
        return m_token_type;
    }
    [[nodiscard]] inline std::string get_lexeme() const
    {
        return m_lexeme;
    }
    [[nodiscard]] inline Value get_literal() const
    {
        return m_literal;
    }
    [[nodiscard]] inline unsigned int get_line() const
    {
        return m_line;
    }
    [[nodiscard]] inline std::string get_str_line() const
    {
        return m_str_line;
    }
    [[nodiscard]] inline unsigned int get_column() const
    {
        return m_column;
    }

private:
    TokenType m_token_type;
    std::string m_lexeme;
    Value m_literal;
    unsigned int m_line;
    // used to print a line that had an error
    std::string m_str_line;
    unsigned int m_column;
};
}

#endif  // TOKEN_TYPE_H