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
    // even though I already matched `FUN` token
    // the current token isn't `FUN`
    // not sure why it happens
    if (match({TokenType::FUN})) return lambda();

    return assignment();
}

ExpressionPtr Parser::lambda()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'fun' in lambda.");
    std::vector<std::shared_ptr<Token>> params;
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (params.size() >= 255) error(peek(), "Can't have more than 255 parameters.");

            auto token = std::make_shared<Token>(consume(TokenType::IDENTIFIER, "Expect parameter name."));
            params.emplace_back(token);
        } while (match({TokenType::COMMA}));
    }

    consume(TokenType::RIGHT_PAREN, "Expect ')' after lambda parameters.");

    consume(TokenType::LEFT_BRACE, "Expect '{' before lambda body.");
    std::vector<StatementPtr> body = block();

    return std::make_shared<expr::Lambda>(params, body);
}

ExpressionPtr Parser::assignment()
{
    ExpressionPtr expr = logic_or();

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

ExpressionPtr Parser::logic_or()
{
    ExpressionPtr expr = logic_and();

    while (match({TokenType::OR}))
    {
        Token op = previous();
        ExpressionPtr right = logic_and();
        expr = std::make_shared<expr::Logical>(expr, op, right);
    }

    return expr;
}

ExpressionPtr Parser::logic_and()
{
    ExpressionPtr expr = equality();

    while (match({TokenType::AND}))
    {
        Token op = previous();
        ExpressionPtr right = equality();
        expr = std::make_shared<expr::Logical>(expr, op, right);
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

    while (match({TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER, TokenType::GREATER_EQUAL}))
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

    return call();
}

ExpressionPtr Parser::call()
{
    ExpressionPtr expr = primary();

    while (true)
    {
        if (match({TokenType::LEFT_PAREN}))
        {
            expr = finish_call(expr);
        }
        else
            break;
    }

    return expr;
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

        try
        {
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        }
        catch (ParseError&)
        {
            synchronize();
        }

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
        if (match({TokenType::FUN})) return function("function");
        if (match({TokenType::VAR})) return var_declaration();

        return statement();
    }
    catch (ParseError& e)
    {
        synchronize();
        return nullptr;
    }
}

StatementPtr Parser::function(const std::string& kind)
{
    // check for lambda function
    if (!check(TokenType::IDENTIFIER)) return statement();

    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");

    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<std::shared_ptr<Token>> params;
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (params.size() >= 255) error(peek(), "Can't have more than 255 parameters.");

            auto token = std::make_shared<Token>(consume(TokenType::IDENTIFIER, "Expect parameter name."));
            params.emplace_back(token);
        } while (match({TokenType::COMMA}));
    }

    consume(TokenType::RIGHT_PAREN, "Expect ')' after " + kind + " parameters.");

    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<StatementPtr> body = block();

    return std::make_shared<stmt::Function>(name, params, body);
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
    if (match({TokenType::FOR})) return for_statement();
    if (match({TokenType::IF})) return if_statement();
    if (match({TokenType::PRINT})) return print_statement();
    if (match({TokenType::RETURN})) return return_statement();
    if (match({TokenType::WHILE})) return while_statement();
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

    while (!check(TokenType::RIGHT_BRACE) && !is_end())
    {
        statements.emplace_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

StatementPtr Parser::if_statement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    ExpressionPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    StatementPtr then = statement();
    std::optional<StatementPtr> else_br = std::nullopt;
    if (match({TokenType::ELSE}))
    {
        else_br = statement();
    }

    return std::make_shared<stmt::If>(condition, then, else_br);
}

StatementPtr Parser::while_statement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    ExpressionPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
    StatementPtr body = statement();

    return std::make_shared<stmt::While>(condition, body);
}

StatementPtr Parser::for_statement()
{
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    std::optional<StatementPtr> initializer;
    if (match({TokenType::SEMICOLON}))
        initializer = std::nullopt;
    else if (match({TokenType::VAR}))
        initializer = var_declaration();
    else
        initializer = expression_statement();

    std::optional<ExpressionPtr> condition = std::nullopt;
    if (!check(TokenType::SEMICOLON)) condition = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after for-loop condition.");

    std::optional<ExpressionPtr> increment = std::nullopt;
    if (!check(TokenType::SEMICOLON)) increment = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    StatementPtr body = statement();

    // convert for loop to a while loop
    // by manually constructing the syntax tree
    if (increment.has_value())
    {
        // we just add the increment expression to the end of the for loop body
        std::vector<StatementPtr> stmts{body, std::make_shared<stmt::Expression>(*increment)};
        body = std::make_shared<stmt::Block>(stmts);
    }

    // if the condition wasn't provided, it is always true
    if (!condition.has_value()) condition = std::make_shared<expr::Literal>(true);
    body = std::make_shared<stmt::While>(*condition, body);

    if (initializer.has_value())
    {
        std::vector<StatementPtr> stmts{*initializer, body};
        body = std::make_shared<stmt::Block>(stmts);
    }

    return body;
}

StatementPtr Parser::return_statement()
{
    Token keyword = previous();
    std::optional<ExpressionPtr> value = std::nullopt;
    if (!check(TokenType::SEMICOLON)) value = expression();

    consume(TokenType::SEMICOLON, "Expect ';' after return values.");
    return std::make_shared<stmt::Return>(keyword, value);
}

ExpressionPtr Parser::finish_call(const ExpressionPtr& callee)
{
    std::vector<ExpressionPtr> args;
    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (args.size() >= 255) error(peek(), "Can't have more than 255 arguments.");

            args.emplace_back(expression());
        } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_shared<expr::Call>(callee, paren, args);
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
