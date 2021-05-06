#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <typeinfo>
#include <vector>

#include "error.h"
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
    ExpressionPtr lambda();
    ExpressionPtr assignment();
    ExpressionPtr logic_or();
    ExpressionPtr logic_and();
    ExpressionPtr equality();
    ExpressionPtr comparison();
    ExpressionPtr term();
    ExpressionPtr factor();
    ExpressionPtr unary();
    ExpressionPtr call();
    ExpressionPtr primary();

    StatementPtr declaration();
    StatementPtr class_declaration();
    // `kind` represents the kind of declaration parsed -
    // a function or a method
    StatementPtr function(const std::string& kind);
    StatementPtr var_declaration();
    StatementPtr statement();
    StatementPtr print_statement();
    StatementPtr expression_statement();
    std::vector<StatementPtr> block();
    StatementPtr if_statement();
    StatementPtr while_statement();
    StatementPtr for_statement();
    StatementPtr return_statement();
    StatementPtr import();

    /*
     * Utility functions
     */

    ExpressionPtr finish_call(const ExpressionPtr& callee);
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
    ParseError error(const Token& token, const std::string& msg);
    // Advances the input to the statement boundary in the process of error recovery
    void synchronize();
    // Check if a prefix has already been added to the function's list of prefixes
    bool is_prefix_added(const std::vector<Token>& prefixes, const Token& prefix) const;

    const std::vector<Token> m_tokens;
    int m_current = 0;
};
}

#endif  // PARSER_H
