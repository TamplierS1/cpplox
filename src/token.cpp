#include "token.h"

namespace garm::types
{

Token::Token(TokenType type, std::string lexeme, OptionalLiteral literal, unsigned int line)
    : m_token_type(type)
    , m_lexeme(std::move(lexeme))
    , m_literal(std::move(literal))
    , m_line(line)
{
}

std::string Token::to_string()
{
    return token_to_string(m_token_type).value_or("ERROR") + " " + m_lexeme + " " + literal_as_string(m_literal.value()).value();
}

std::optional<std::string> Token::token_to_string(TokenType token)
{
    static std::map<TokenType, std::string> token_lookup{
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::SLASH, "SLASH"},
        {TokenType::STAR, "STAR"},
        {TokenType::BANG, "BANG"},
        {TokenType::BANG_EQUAL, "BANG_EQUAL"},
        {TokenType::EQUAL, "EQUAL"},
        {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
        {TokenType::GREATER, "GREATER"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::LESS, "LESS"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::PLUS, "PLUS"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::STRING, "STRING"},
        {TokenType::NUMBER, "NUMBER"},
        {TokenType::AND, "AND"},
        {TokenType::CLASS, "CLASS"},
        {TokenType::ELSE, "ELSE"},
        {TokenType::FALSE, "FALSE"},
        {TokenType::FUN, "FUN"},
        {TokenType::FOR, "FOR"},
        {TokenType::IF, "IF"},
        {TokenType::NIL, "NIL"},
        {TokenType::OR, "OR"},
        {TokenType::PRINT, "PRINT"},
        {TokenType::RETURN, "RETURN"},
        {TokenType::SUPER, "SUPER"},
        {TokenType::THIS, "THIS"},
        {TokenType::TRUE, "TRUE"},
        {TokenType::VAR, "VAR"},
        {TokenType::WHILE, "WHILE"},
        {TokenType::GARM_EOF, "EOF"}
    };

    if (token_lookup.contains(token))
    {
        return token_lookup.find(token)->second;
    }
    else
    {
        ErrorHandler::get_instance().debug_error("Invalid token.");
        return std::nullopt;
    }
}

TokenType Token::get_token_type() const
{
    return m_token_type;
}

std::string Token::get_lexeme() const
{
    return m_lexeme;
}

OptionalLiteral Token::get_literal() const
{
    return m_literal;
}

unsigned int Token::get_line() const
{
    return m_line;
}

}  // namespace garm::types