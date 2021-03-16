#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include <utility>
#include <map>

#include "literal.h"

namespace garm::types
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

    GARM_EOF
};

class Token
{
public:
    Token(TokenType type, std::string lexeme, OptionalLiteral literal, unsigned int line);

    std::string to_string();

    [[nodiscard]] TokenType get_token_type() const;
    [[nodiscard]] std::string get_lexeme() const;
    [[nodiscard]] OptionalLiteral get_literal() const;
    [[nodiscard]] unsigned int get_line() const;
private:
    static std::optional<std::string> token_to_string(TokenType token);

    TokenType m_token_type;
    std::string m_lexeme;
    OptionalLiteral m_literal = std::nullopt;
    unsigned int m_line;

};
}  // namespace garm::types

#endif  // TOKEN_TYPE_H