#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>
#include <typeinfo>

#include "error_handler.h"
#include "syntax_tree/expression.h"
#include "syntax_tree/statement.h"
#include "token.h"

namespace cpplox
{

using namespace ast;

// Parser that uses recursive decent parsing method
class Parser
{
public:
    explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens))
    {
    }

    // Begins parsing
    std::optional<std::vector<StatementPtr>> parse();

private:
    /*
     * Methods that directly translate to the grammar rules
     */

    ExpressionPtr expression();
    ExpressionPtr assignment();
    ExpressionPtr equality();
    ExpressionPtr comparison();
    ExpressionPtr term();
    ExpressionPtr factor();
    ExpressionPtr unary();
    ExpressionPtr primary();

    StatementPtr declaration();
    StatementPtr var_declaration();
    StatementPtr statement();
    StatementPtr print_statement();
    StatementPtr expression_statement();
    std::vector<StatementPtr> block();

    /*
     * Utility functions
     */

    // Checks if the current token is of any of the given types. Consumes the token if the type matches
    bool match(const std::vector<TokenType>&& types);
    // Returns the current token and consumes it.
    Token advance();
    // Checks if the current token is of the given type, consumes it and returns it.
    // If the types don't match an error is reported
    Token consume(TokenType type, const std::string& msg);
    // Checks if current token is of the given type.
    [[nodiscard]] bool check(TokenType type) const;
    // Returns the current token
    [[nodiscard]] Token peek() const;
    // Checks if the input has exhausted
    [[nodiscard]] bool is_end() const;
    // Returns the most recently consumed token
    [[nodiscard]] Token previous() const;
    // Reports an error and returns an exception
    [[nodiscard]] ParseError error(const Token& token, const std::string& msg);
    // Advances the input to the statement boundary in the process of error recovery
    void synchronize();

    const std::vector<Token> m_tokens;
    unsigned int m_current = 0;
};
}

#endif  // PARSER_H