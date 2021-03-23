#include "parser.h"

namespace garm
{
std::optional<ExpressionPtr> Parser::parse()
{
    try
    {
        return expression();
    }
    catch (ParseError&)
    {
        return std::nullopt;
    }
}

ExpressionPtr Parser::expression()
{
    return equality();
}

ExpressionPtr Parser::equality()
{
    ExpressionPtr expr = comparison();

    while (match({types::TokenType::BANG_EQUAL, types::TokenType::EQUAL_EQUAL}))
    {
        types::Token op = previous();
        ExpressionPtr right = comparison();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }

    return expr;
}

ExpressionPtr Parser::comparison()
{
    ExpressionPtr expr = term();

    while (match({types::TokenType::LESS, types::TokenType::LESS_EQUAL, types::TokenType::GREATER,
                  types::TokenType::GREATER_EQUAL}))
    {
        types::Token op = previous();
        ExpressionPtr right = term();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }

    return expr;
}

ExpressionPtr Parser::term()
{
    ExpressionPtr expr = factor();

    while (match({types::TokenType::PLUS, types::TokenType::MINUS}))
    {
        types::Token op = previous();
        ExpressionPtr right = factor();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }

    return expr;
}

ExpressionPtr Parser::factor()
{
    ExpressionPtr expr = unary();

    while (match({types::TokenType::SLASH, types::TokenType::STAR}))
    {
        types::Token op = previous();
        ExpressionPtr right = unary();
        expr = std::make_shared<ast::Binary>(expr, op, right);
    }

    return expr;
}

ExpressionPtr Parser::unary()
{
    if (match({types::TokenType::BANG, types::TokenType::MINUS}))
    {
        types::Token op = previous();
        ExpressionPtr right = unary();

        return std::make_shared<ast::Unary>(op, right);
    }

    return primary();
}

ExpressionPtr Parser::primary()
{
    if (match({types::TokenType::FALSE})) return std::make_shared<ast::Literal>(false);
    if (match({types::TokenType::TRUE})) return std::make_shared<ast::Literal>(true);
    if (match({types::TokenType::NIL})) return std::make_shared<ast::Literal>(std::nullopt);

    if (match({types::TokenType::STRING, types::TokenType::NUMBER}))
    {
        return std::make_shared<ast::Literal>(previous().get_literal());
    }

    if (match({types::TokenType::LEFT_PAREN}))
    {
        ExpressionPtr mid_expr = expression();

        consume(types::TokenType::RIGHT_PAREN, "Expect ')' after expression.");

        // TODO: uncomment when you implement statements
        /*try {
            consume(types::TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        }
        catch (ParseError&)
        {
            synchronize();
        }*/

        return std::make_shared<ast::Grouping>(mid_expr);
    }

    // Failed to find any rule to parse the token
    throw error(peek(), "Expect expression.");
}

bool Parser::match(const std::vector<types::TokenType>&& types)
{
    for (const auto& type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }

    return false;
}

types::Token Parser::advance()
{
    if (!is_end()) m_current++;
    return previous();
}

types::Token Parser::consume(types::TokenType type, const std::string& msg)
{
    if (check(type)) return advance();

    throw error(peek(), msg);
}

bool Parser::check(types::TokenType type) const
{
    if (is_end()) return false;
    return peek().get_token_type() == type;
}

types::Token Parser::peek() const
{
    return m_tokens.at(m_current);
}

bool Parser::is_end() const
{
    return peek().get_token_type() == types::TokenType::GARM_EOF;
}

types::Token Parser::previous() const
{
    return m_tokens.at(m_current - 1);
}

ParseError Parser::error(const types::Token& token, const std::string& msg)
{
    ErrorHandler::get_instance().error(token, msg);
    return ParseError{};
}

void Parser::synchronize()
{
    advance();

    while (!is_end())
    {
        if (previous().get_token_type() == types::TokenType::SEMICOLON) return;

        switch (peek().get_token_type())
        {
            case types::TokenType::CLASS:
            case types::TokenType::FUN:
            case types::TokenType::VAR:
            case types::TokenType::FOR:
            case types::TokenType::IF:
            case types::TokenType::WHILE:
            case types::TokenType::PRINT:
            case types::TokenType::RETURN:
                return;
        }

        advance();
    }
}

}
