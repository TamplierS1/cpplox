#include "parser.h"

namespace cpplox
{
std::optional<std::vector<StatementPtr>> Parser::parse()
{
    std::vector<StatementPtr> statements;
    while (!is_end())
    {
        statements.emplace_back(declaration());
    }

    if (statements.empty()) return std::nullopt;

    return statements;
}

ExpressionPtr Parser::expression()
{
    return assignment();
}

ExpressionPtr Parser::assignment()
{
    ExpressionPtr expr = equality();

    if (match({TokenType::EQUAL}))
    {
        Token equals = previous();
        ExpressionPtr value = assignment();

        if (typeid(*expr) == typeid(expr::Variable))
        {
            Token name = *std::dynamic_pointer_cast<expr::Variable>(expr)->m_name;
            return std::make_shared<expr::Assign>(name, value);
        }

        // no need to throw an exception here,
        // because the parser is not in a confused state
        error(equals, "Invalid argument target.");
    }

    return expr;
}

ExpressionPtr Parser::equality()
{
    ExpressionPtr expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
    {
        Token op = previous();
        ExpressionPtr right = comparison();
        expr = std::make_shared<expr::Binary>(expr, op, right);
    }

    return expr;
}

ExpressionPtr Parser::comparison()
{
    ExpressionPtr expr = term();

    while (match({TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER,
                  TokenType::GREATER_EQUAL}))
    {
        Token op = previous();
        ExpressionPtr right = term();
        expr = std::make_shared<expr::Binary>(expr, op, right);
    }

    return expr;
}

ExpressionPtr Parser::term()
{
    ExpressionPtr expr = factor();

    while (match({TokenType::PLUS, TokenType::MINUS}))
    {
        Token op = previous();
        ExpressionPtr right = factor();
        expr = std::make_shared<expr::Binary>(expr, op, right);
    }

    return expr;
}

ExpressionPtr Parser::factor()
{
    ExpressionPtr expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR}))
    {
        Token op = previous();
        ExpressionPtr right = unary();
        expr = std::make_shared<expr::Binary>(expr, op, right);
    }

    return expr;
}

ExpressionPtr Parser::unary()
{
    if (match({TokenType::BANG, TokenType::MINUS}))
    {
        Token op = previous();
        ExpressionPtr right = unary();

        return std::make_shared<expr::Unary>(op, right);
    }

    return primary();
}

ExpressionPtr Parser::primary()
{
    if (match({TokenType::FALSE})) return std::make_shared<expr::Literal>(false);
    if (match({TokenType::TRUE})) return std::make_shared<expr::Literal>(true);
    if (match({TokenType::NIL})) return std::make_shared<expr::Literal>(std::nullopt);

    if (match({TokenType::STRING, TokenType::NUMBER}))
    {
        return std::make_shared<expr::Literal>(previous().get_literal());
    }

    if (match({TokenType::IDENTIFIER}))
    {
        return std::make_shared<expr::Variable>(previous());
    }

    if (match({TokenType::LEFT_PAREN}))
    {
        ExpressionPtr mid_expr = expression();

        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");

        // TODO: uncomment when you implement statements
        /*try {
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        }
        catch (ParseError&)
        {
            synchronize();
        }*/

        return std::make_shared<expr::Grouping>(mid_expr);
    }

    /* TODO: I think throwing an exception to unwind wasn't the best idea.
     *  Come up with something else
     */
    // Failed to find any rule to parse the token
    throw error(peek(), "Expect expression.");
}

StatementPtr Parser::declaration()
{
    try
    {
        if (match({TokenType::VAR}))
            return var_declaration();

        return statement();
    }
    catch(ParseError& e)
    {
        synchronize();
        return nullptr;
    }
}

StatementPtr Parser::var_declaration()
{
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::optional<ExpressionPtr> initializer = std::nullopt;
    if (match({TokenType::EQUAL}))
    {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<stmt::Var>(name, initializer);
}

StatementPtr Parser::statement()
{
    if (match({TokenType::PRINT})) return print_statement();
    if (match({TokenType::LEFT_BRACE})) return std::make_shared<stmt::Block>(block());

    return expression_statement();
}

StatementPtr Parser::print_statement()
{
    ExpressionPtr value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_shared<stmt::Print>(value);
}

StatementPtr Parser::expression_statement()
{
    ExpressionPtr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<stmt::Expression>(expr);
}

std::vector<StatementPtr> Parser::block()
{
    std::vector<StatementPtr> statements;

    while(!check(TokenType::RIGHT_BRACE) && !is_end())
    {
        statements.emplace_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

bool Parser::match(const std::vector<TokenType>&& types)
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

Token Parser::advance()
{
    if (!is_end()) m_current++;
    return previous();
}

Token Parser::consume(TokenType type, const std::string& msg)
{
    if (check(type)) return advance();

    throw error(peek(), msg);
}

bool Parser::check(TokenType type) const
{
    if (is_end()) return false;
    return peek().get_token_type() == type;
}

Token Parser::peek() const
{
    return m_tokens.at(m_current);
}

bool Parser::is_end() const
{
    return peek().get_token_type() == TokenType::cpplox_EOF;
}

Token Parser::previous() const
{
    return m_tokens.at(m_current - 1);
}

ParseError Parser::error(const Token& token, const std::string& msg)
{
    ErrorHandler::get_instance().error(token, msg);
    return ParseError{};
}

void Parser::synchronize()
{
    advance();

    while (!is_end())
    {
        if (previous().get_token_type() == TokenType::SEMICOLON) return;

        switch (peek().get_token_type())
        {
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
        }

        advance();
    }
}

}
